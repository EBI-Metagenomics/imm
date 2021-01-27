#include "cass/cass.h"
#include "elapsed/elapsed.h"
#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

struct model
{
    struct imm_hmm*                hmm;
    struct imm_abc const*          abc;
    struct imm_vecp*               mute_states;
    struct imm_vecp*               normal_states;
    struct imm_normal_state const* end;
};

void test_window_viterbi(void);

int main(void)
{
    test_window_viterbi();
    return cass_status();
}

static struct model     create_model(void);
static void             destroy_model(struct model model);
static char*            fmt_name(char* restrict buffer, char const* name, int i);
static inline imm_float zero(void) { return imm_lprob_zero(); }

void test_window_viterbi(void)
{
    struct model model = create_model();

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
    struct imm_seq const* seq = imm_seq_create(str, model.abc);
    struct imm_dp const*  dp = imm_hmm_create_dp(model.hmm, imm_normal_state_super(model.end));
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 50);
    struct imm_results const* results = imm_dp_viterbi(dp, task);

    cass_cond(imm_results_size(results) == 79);

    struct imm_result const* result = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(result);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_close(score, -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 1);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "MIMMMMMMMMMIIMIMIMIMIMIIMIIIMIMIMIMMMMMMIMMIMIMIMI",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 2);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 3);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_cond(!imm_lprob_is_valid(score));
    cass_cond(strncmp(imm_seq_string(s), "IMIMMIMIMIMIMIMMMMIMMIMEJBMIIMIIMMIMMMIMEJBMIIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 4);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
    cass_close(score, -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 8);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_likelihood(model.hmm, s, imm_result_path(result));
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
    imm_dp_destroy(dp);
    destroy_model(model);
}

static char* fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

static struct model create_model(void)
{
    int          ncore_nodes = 1000;
    struct model m = {.hmm = NULL,
                      .abc = NULL,
                      .mute_states = imm_vecp_create(),
                      .normal_states = imm_vecp_create()};

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    m.abc = abc;
    m.hmm = hmm;

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), log(1.0));
    imm_vecp_append(m.mute_states, start);

    imm_float end_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05), log(0.05)};
    struct imm_normal_state const* end = imm_normal_state_create("END", abc, end_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(end), zero());
    m.end = end;

    imm_float B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    imm_float E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    imm_float J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    imm_float M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    imm_float I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(B), zero());
    imm_vecp_append(m.normal_states, B);
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(E), zero());
    imm_vecp_append(m.normal_states, E);
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(J), zero());
    imm_vecp_append(m.normal_states, J);

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
        imm_vecp_append(m.normal_states, M[i]);
        imm_hmm_add_state(hmm, imm_normal_state_super(I[i]), zero());
        imm_vecp_append(m.normal_states, I[i]);
        imm_hmm_add_state(hmm, imm_mute_state_super(D[i]), zero());
        imm_vecp_append(m.normal_states, D[i]);

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

    return m;
}

static void destroy_model(struct model model)
{
    for (size_t i = 0; i < imm_vecp_length(model.mute_states); ++i) {
        struct imm_mute_state const* s = imm_vecp_get(model.mute_states, i);
        imm_mute_state_destroy(s);
    }

    for (size_t i = 0; i < imm_vecp_length(model.normal_states); ++i) {
        struct imm_normal_state const* s = imm_vecp_get(model.normal_states, i);
        imm_normal_state_destroy(s);
    }

    imm_normal_state_destroy(model.end);
    imm_abc_destroy(model.abc);
    imm_hmm_destroy(model.hmm);
    imm_vecp_destroy(model.mute_states);
    imm_vecp_destroy(model.normal_states);
}
