#include "cass/cass.h"
#include "imm/imm.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 100

imm_float perf_multi_thread_viterbi(double* seconds, uint16_t ncore_nodes, uint16_t seq_100length);

int main(void)
{
    /* imm_float const logliks[] = { */
    /*     -394.3122885464,    -3943.1228854637,   -7886.2457709276,   -11829.3686563895, */
    /*     -15772.4915418512,  -19715.6144273129,  -23658.7373127746,  -27601.8601982363, */
    /*     -31544.9830836981,  -35488.1059691598,  -39431.2288546215,  -1681.8626184936, */
    /*     -16818.6261849329,  -33637.2523698614,  -50455.8785547899,  -67274.5047397263, */
    /*     -84093.1309247308,  -100911.7571097354, -117730.3832947399, -134549.0094797444, */
    /*     -151367.6356647490, -168186.2618497535, -3291.3005309278,   -32913.0053092664, */
    /*     -65826.0106185297,  -98739.0159279405,  -131652.0212373513, -164565.0265467621, */
    /*     -197478.0318561729, -230391.0371655837, -263304.0424749735, -296217.0477837892, */
    /*     -329130.0530926048, */
    /* }; */

    printf("ncore_nodes,seq_length,median,std_err_mean,score,perf_name\n");

    uint16_t ncore_nodes[] = {100, 500, 1000};
    uint16_t seq_100len[] = {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(ncore_nodes); ++i) {
        for (unsigned j = 0; j < IMM_ARRAY_SIZE(seq_100len); ++j) {
            uint16_t  len = seq_100len[j];
            double    seconds[NSAMPLES] = {0.};
            imm_float loglik = perf_multi_thread_viterbi(seconds, ncore_nodes[i], len);
            /* cass_close(loglik, logliks[i * IMM_ARRAY_SIZE(seq_100len) + j]); */
            struct stats stats = compute_stats(seconds, NSAMPLES);
            char const   fmt[] = "%d,%d,%.6f,%.6f,%.6f,1thread_viterbi\n";
            printf(fmt, ncore_nodes[i], len * 100, stats.median, stats.sem, loglik);
        }
    }
    return 0;
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int       is_valid(imm_float a) { return imm_lprob_is_valid(a); }
static inline int       is_zero(imm_float a) { return imm_lprob_is_zero(a); }
static inline char*     fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

imm_float perf_multi_thread_viterbi(imm_float* seconds, uint16_t ncore_nodes,
                                    uint16_t seq_100length)
{
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
    for (uint16_t i = 0; i < ncore_nodes; ++i) {
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
    char* str = malloc(sizeof(*str) * (unsigned)(100 * seq_100length + 1));
    str[100 * seq_100length] = '\0';
    char const str0[] = "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMMMMMMMIIMIMIMIMIMIIM";
    char const str1[] = "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMEJ";
    char const str_end[] = "IIIMIMIMIMMMMMMIMMIMIMIMIIMIMMIMIMIMIMIMMMMIMMIMME";
    for (uint16_t i = 0; i < seq_100length; ++i) {
        for (uint16_t j = 0; j < 50; ++j) {
            str[i * 100 + j] = str0[j];
            if (i < seq_100length - 1)
                str[i * 100 + 50 + j] = str1[j];
            else
                str[i * 100 + 50 + j] = str_end[j];
        }
    }
    cass_cond(strlen(str) == 100 * seq_100length);

    struct imm_seq const* seq = imm_seq_create(str, abc);
    struct imm_dp const*  dp = imm_hmm_create_dp(hmm, imm_mute_state_super(end));

    imm_float           loglik = 0.0;
    struct imm_dp_task* task = imm_dp_task_create(dp);
    for (unsigned i = 0; i < NSAMPLES; ++i) {
        imm_dp_task_setup(task, seq, 2 * ncore_nodes);
        struct imm_results const* results = imm_dp_viterbi(dp, task);
        /* cass_cond(imm_results_size(results) == 1); */
        /* struct imm_result const* r = imm_results_get(results, 0); */
        /* struct imm_subseq        subseq = imm_result_subseq(r); */
        /* struct imm_seq const*    s = imm_subseq_cast(&subseq); */
        /* loglik = imm_hmm_likelihood(hmm, s, imm_result_path(r)); */
        /* cass_cond(is_valid(loglik) && !is_zero(loglik)); */
        seconds[i] = imm_results_seconds(results);
        imm_results_destroy(results);
    }

    imm_mute_state_destroy(start);
    imm_normal_state_destroy(B);
    for (uint16_t i = 0; i < ncore_nodes; ++i) {
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
    imm_dp_task_destroy(task);
    free((void*)str);

    return loglik;
}
