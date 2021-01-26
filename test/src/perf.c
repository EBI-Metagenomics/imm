#include "cass/cass.h"
#include "elapsed/elapsed.h"
#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

void test_perf_viterbi(void);

int main(void)
{
    test_perf_viterbi();
    return cass_status();
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int       is_valid(imm_float a) { return imm_lprob_is_valid(a); }
static inline int       is_zero(imm_float a) { return imm_lprob_is_zero(a); }
static inline char*     fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

void test_perf_viterbi(void)
{
    int ncore_nodes = 1000;

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), log(1.0));

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(end), zero());

    imm_float B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    imm_float E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    imm_float J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    imm_float M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    imm_float I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(B), zero());
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(E), zero());
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(J), zero());

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(E), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(J), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(J), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(B), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_mute_state_super(end), log(0.2));

    struct imm_normal_state const* M[ncore_nodes];
    struct imm_normal_state const* I[ncore_nodes];
    struct imm_mute_state const*   D[ncore_nodes];

    char name[10] = "\0";
    for (int i = 0; i < ncore_nodes; ++i) {
        M[i] = imm_normal_state_create(fmt_name(name, "M", i), abc, M_lprobs);
        I[i] = imm_normal_state_create(fmt_name(name, "I", i), abc, I_lprobs);
        D[i] = imm_mute_state_create(fmt_name(name, "D", i), abc);

        imm_hmm_add_state(hmm, imm_normal_state_super(M[i]), zero());
        imm_hmm_add_state(hmm, imm_normal_state_super(I[i]), zero());
        imm_hmm_add_state(hmm, imm_mute_state_super(D[i]), zero());

        if (i == 0)
            imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M[0]),
                              log(0.2));

        imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(I[i]),
                          log(0.2));
        imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(I[i]),
                          log(0.2));

        if (i > 0) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i - 1]), imm_normal_state_super(M[i]),
                              log(0.2));

            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_mute_state_super(D[i]),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_mute_state_super(D[i]),
                              log(0.2));
        }

        if (i == ncore_nodes - 1) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(E),
                              log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i]), imm_normal_state_super(E), log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(E),
                              log(0.2));
        }
    }

    struct elapsed elapsed = elapsed_init();

    char const str[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";
    cass_cond(strlen(str) == 2000);

    struct imm_seq const* seq = imm_seq_create(str, abc);
    elapsed_start(&elapsed);
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, imm_mute_state_super(end));
    struct imm_dp_task*  task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 0);
    struct imm_results const* results = imm_dp_viterbi(dp, task);
    elapsed_end(&elapsed);

    cass_cond(imm_results_size(results) == 1);
    struct imm_result const* r = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(r);
    imm_float score = imm_hmm_likelihood(hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(is_valid(score) && !is_zero(score));
    cass_close(score, -65826.0106185297);
    imm_results_destroy(results);

    results = imm_dp_viterbi(dp, task);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    score = imm_hmm_likelihood(hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(is_valid(score) && !is_zero(score));
    cass_close(score, -65826.0106185297);
    imm_results_destroy(results);

    results = imm_dp_viterbi(dp, task);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    score = imm_hmm_likelihood(hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(is_valid(score) && !is_zero(score));
    cass_close(score, -65826.0106185297);
    imm_results_destroy(results);

    struct imm_output* output = imm_output_create(TMPDIR "/perf.imm");
    cass_cond(output != NULL);
    struct imm_model const* model = imm_model_create(hmm, dp);
    cass_equal_int(imm_output_write(output, model), 0);
    imm_model_destroy(model);
    cass_equal_int(imm_output_destroy(output), 0);

    imm_seq_destroy(seq);
    imm_dp_task_destroy(task);

#ifdef NDEBUG
    cass_cond(elapsed_seconds(&elapsed) < 5.0);
#endif

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(B);
    for (int i = 0; i < ncore_nodes; ++i) {
        imm_normal_state_destroy(M[i]);
        imm_normal_state_destroy(I[i]);
        imm_mute_state_destroy(D[i]);
    }
    imm_normal_state_destroy(J);
    imm_normal_state_destroy(E);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_dp_destroy(dp);

    struct imm_input* input = imm_input_create(TMPDIR "/perf.imm");
    cass_cond(input != NULL);
    model = imm_input_read(input);
    cass_cond(model != NULL);
    cass_equal_int(imm_input_destroy(input), 0);

    abc = imm_model_abc(model);
    hmm = imm_model_hmm(model);
    dp = imm_model_dp(model);

    seq = imm_seq_create(str, abc);
    task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 0);
    results = imm_dp_viterbi(dp, task);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    score = imm_hmm_likelihood(hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(is_valid(score) && !is_zero(score));
    cass_close2(score, -65826.0106185297, 1e-07, 0.0);
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_model_nstates(model); ++i)
        imm_state_destroy(imm_model_state(model, i));

    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_seq_destroy(seq);
    imm_dp_task_destroy(task);
    imm_model_destroy(model);
}
