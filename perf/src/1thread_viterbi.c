#include "cass/cass.h"
#include "imm/imm.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 100

void perf_1thread_viterbi(double* seconds);

int main(void)
{
    double seconds[NSAMPLES] = {0.};
    perf_1thread_viterbi(seconds);
    struct stats stats = compute_stats(seconds, NSAMPLES);
    printf("Avg: %.6f (+/- %.6f) seconds\n", stats.mean, stats.sem);
    return 0;
}

static inline double zero(void) { return imm_lprob_zero(); }
static inline int    is_valid(double a) { return imm_lprob_is_valid(a); }
static inline int    is_zero(double a) { return imm_lprob_is_zero(a); }
static inline char*  fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

void perf_1thread_viterbi(double* seconds)
{
    int ncore_nodes = 1000;

    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), log(1.0));

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(end), zero());

    double B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    double E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    double J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    double M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    double I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

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
    struct imm_dp const*  dp = imm_hmm_create_dp(hmm, imm_mute_state_super(end));

    for (unsigned i = 0; i < NSAMPLES; ++i) {
        struct imm_results const* results = imm_dp_viterbi(dp, seq, 0);
        cass_cond(imm_results_size(results) == 1);
        struct imm_result const* r = imm_results_get(results, 0);
        double                   score = imm_result_loglik(r);
        cass_cond(is_valid(score) && !is_zero(score));
        cass_close(score, -65823.5546875000);
        seconds[i] = imm_result_seconds(r);
        imm_results_destroy(results);
    }

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
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_seq_destroy(seq);
}
