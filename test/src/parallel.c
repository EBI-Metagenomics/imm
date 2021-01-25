#include "cass/cass.h"
#include "elapsed/elapsed.h"
#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

void test_parallel(void);

int main(void)
{
    test_parallel();
    return cass_status();
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline char*     fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

void test_parallel(void)
{
    int ncore_nodes = 1000;

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), log(1.0));

    imm_float end_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05), log(0.05)};
    struct imm_normal_state const* end = imm_normal_state_create("END", abc, end_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(end), zero());

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
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(end), log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(end), imm_normal_state_super(end), log(0.2));

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

    struct imm_path* path = imm_path_create();

    char const str[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ"
                       "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM"
                       "BIIIMIMIMIMMMMMMIMMIMMMMMIIMIMMIMIMIMIMIMMMIMMIMME"
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

    elapsed_start(&elapsed);
    struct imm_seq const* seq = imm_seq_create(str, abc);
    struct imm_dp const*  dp = imm_hmm_create_dp(hmm, imm_normal_state_super(end));
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    struct imm_results const* results = imm_dp_viterbi(dp, task, seq, 50);

    cass_cond(imm_results_size(results) == 79);

    struct imm_result const* result = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(result);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_close(score, -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 1);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "MIMMMMMMMMMIIMIMIMIMIMIIMIIIMIMIMIMMMMMMIMMIMIMIMI",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 2);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 3);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "IMIMMIMIMIMIMIMMMMIMMIMEJBMIIMIIMMIMMMIMEJBMIIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 4);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_close(score, -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 8);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(result));
    cass_close(score, -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    elapsed_end(&elapsed);
    imm_path_destroy(path);
    imm_seq_destroy(seq);
    imm_dp_task_destroy(task);

#ifdef NDEBUG
    cass_cond(elapsed_seconds(&elapsed) < 5.0);
#endif

    imm_results_destroy(results);
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
    imm_normal_state_destroy(end);
    imm_abc_destroy(abc);
    imm_dp_destroy(dp);
}
