#include "cass.h"
#include "elapsed.h"
#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

void test_parallel(void);

int main(void)
{
    test_parallel();
    return cass_status();
}

static inline double                  zero(void) { return imm_lprob_zero(); }
static inline struct imm_state const* cast_c(void const* s) { return imm_state_cast_c(s); }
static inline char*                   fmt_name(char* restrict buffer, char const* name, int i)
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
    imm_hmm_add_state(hmm, cast_c(start), log(1.0));

    double end_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05), log(0.05)};
    struct imm_normal_state const* end = imm_normal_state_create("END", abc, end_lprobs);
    imm_hmm_add_state(hmm, cast_c(end), zero());

    double B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    double E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    double J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    double M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    double I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, cast_c(B), zero());
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, cast_c(E), zero());
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, cast_c(J), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(E), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(J), cast_c(J), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(J), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(J), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(end), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(end), cast_c(end), log(0.2));

    struct imm_normal_state const* M[ncore_nodes];
    struct imm_normal_state const* I[ncore_nodes];
    struct imm_mute_state const*   D[ncore_nodes];

    char name[10] = "\0";
    for (int i = 0; i < ncore_nodes; ++i) {
        M[i] = imm_normal_state_create(fmt_name(name, "M", i), abc, M_lprobs);
        I[i] = imm_normal_state_create(fmt_name(name, "I", i), abc, I_lprobs);
        D[i] = imm_mute_state_create(fmt_name(name, "D", i), abc);

        imm_hmm_add_state(hmm, cast_c(M[i]), zero());
        imm_hmm_add_state(hmm, cast_c(I[i]), zero());
        imm_hmm_add_state(hmm, cast_c(D[i]), zero());

        if (i == 0)
            imm_hmm_set_trans(hmm, cast_c(B), cast_c(M[0]), log(0.2));

        imm_hmm_set_trans(hmm, cast_c(M[i]), cast_c(I[i]), log(0.2));
        imm_hmm_set_trans(hmm, cast_c(I[i]), cast_c(I[i]), log(0.2));

        if (i > 0) {
            imm_hmm_set_trans(hmm, cast_c(M[i - 1]), cast_c(M[i]), log(0.2));
            imm_hmm_set_trans(hmm, cast_c(D[i - 1]), cast_c(M[i]), log(0.2));
            imm_hmm_set_trans(hmm, cast_c(I[i - 1]), cast_c(M[i]), log(0.2));

            imm_hmm_set_trans(hmm, cast_c(M[i - 1]), cast_c(D[i]), log(0.2));
            imm_hmm_set_trans(hmm, cast_c(D[i - 1]), cast_c(D[i]), log(0.2));
        }

        if (i == ncore_nodes - 1) {
            imm_hmm_set_trans(hmm, cast_c(M[i]), cast_c(E), log(0.2));
            imm_hmm_set_trans(hmm, cast_c(D[i]), cast_c(E), log(0.2));
            imm_hmm_set_trans(hmm, cast_c(I[i]), cast_c(E), log(0.2));
        }
    }

    struct elapsed* elapsed = elapsed_create();

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

    elapsed_start(elapsed);
    struct imm_seq const*     seq = imm_seq_create(str, abc);
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, cast_c(end));
    struct imm_results const* results = imm_dp_viterbi(dp, seq, 50);

    cass_cond(imm_results_size(results) == 79);

    struct imm_result const* result = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(result);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    cass_close(imm_result_loglik(result), -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 1);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    cass_cond(!imm_lprob_is_valid(imm_result_loglik(result)));
    cass_cond(strncmp(imm_seq_string(s), "MIMMMMMMMMMIIMIMIMIMIMIIMIIIMIMIMIMMMMMMIMMIMIMIMI",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 2);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    cass_cond(!imm_lprob_is_valid(imm_result_loglik(result)));
    cass_cond(strncmp(imm_seq_string(s), "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 3);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    cass_cond(!imm_lprob_is_valid(imm_result_loglik(result)));
    cass_cond(strncmp(imm_seq_string(s), "IMIMMIMIMIMIMIMMMMIMMIMEJBMIIMIIMMIMMMIMEJBMIIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 4);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    cass_close(imm_result_loglik(result), -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    result = imm_results_get(results, 8);
    subseq = imm_result_subseq(result);
    s = imm_subseq_cast(&subseq);
    cass_close(imm_result_loglik(result), -1778.8892020572);
    cass_cond(strncmp(imm_seq_string(s), "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM",
                      imm_seq_length(s)) == 0);

    elapsed_end(elapsed);
    imm_path_destroy(path);
    imm_seq_destroy(seq);

#ifdef NDEBUG
    cass_cond(elapsed_seconds(elapsed) < 1.0);
#endif

    elapsed_destroy(elapsed);
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
