#include "cass/cass.h"
#include "imm/imm.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 100

#ifdef IMM_DOUBLE_PRECISION
#define CLOSE(x, y) cass_close2(x, y, 1e-9, 0.0)
#else
#define CLOSE(x, y) cass_close2(x, y, 1e-2, 0.0)
#endif

imm_float perf_1thread_viterbi(imm_float* seconds, uint16_t ncore_nodes, uint16_t seq_100length);

int main(void)
{
    imm_float const logliks[] = {
        (imm_float)-394.3122885464,    (imm_float)-3943.1228854637,   (imm_float)-7886.2457709276,
        (imm_float)-11829.3686563895,  (imm_float)-15772.4915418512,  (imm_float)-19715.6144273129,
        (imm_float)-23658.7373127746,  (imm_float)-27601.8601982363,  (imm_float)-31544.9830836981,
        (imm_float)-35488.1059691598,  (imm_float)-39431.2288546215,  (imm_float)-1681.8626184936,
        (imm_float)-16818.6261849329,  (imm_float)-33637.2523698614,  (imm_float)-50455.8785547899,
        (imm_float)-67274.5047397263,  (imm_float)-84093.1309247308,  (imm_float)-100911.7571097354,
        (imm_float)-117730.3832947399, (imm_float)-134549.0094797444, (imm_float)-151367.6356647490,
        (imm_float)-168186.2618497535, (imm_float)-3291.3005309278,   (imm_float)-32913.0053092664,
        (imm_float)-65826.0106185297,  (imm_float)-98739.0159279405,  (imm_float)-131652.0212373513,
        (imm_float)-164565.0265467621, (imm_float)-197478.0318561729, (imm_float)-230391.0371655837,
        (imm_float)-263304.0424749735, (imm_float)-296217.0477837892, (imm_float)-329130.0530926048,
    };

    printf("ncore_nodes,seq_length,median,std_err_mean,score,perf_name\n");

    uint16_t ncore_nodes[] = {100, 500, 1000};
    uint16_t seq_100len[] = {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(ncore_nodes); ++i) {
        for (unsigned j = 0; j < IMM_ARRAY_SIZE(seq_100len); ++j) {
            uint16_t  len = seq_100len[j];
            imm_float seconds[NSAMPLES] = {0.};
            imm_float loglik = perf_1thread_viterbi(seconds, ncore_nodes[i], len);
            CLOSE(loglik, logliks[i * IMM_ARRAY_SIZE(seq_100len) + j]);
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

imm_float perf_1thread_viterbi(imm_float* seconds, uint16_t ncore_nodes, uint16_t seq_100length)
{
    struct imm_abc const* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start), imm_log(1.0));

    struct imm_mute_state const* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(end), zero());

    imm_float B_lprobs[] = {imm_log(1.0), zero(), zero(), zero(), zero()};
    imm_float E_lprobs[] = {zero(), zero(), zero(), imm_log(1.0), zero()};
    imm_float J_lprobs[] = {zero(), zero(), zero(), zero(), imm_log(1.0)};
    imm_float M_lprobs[] = {zero(), imm_log(1.0), zero(), zero(), zero()};
    imm_float I_lprobs[] = {zero(), zero(), imm_log(1.0), zero(), zero()};

    struct imm_normal_state const* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(B), zero());
    struct imm_normal_state const* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(E), zero());
    struct imm_normal_state const* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(J), zero());

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(B), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(B), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(E), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(J), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_normal_state_super(J), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(J), imm_normal_state_super(B), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(E), imm_mute_state_super(end), imm_log(0.2));

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
            imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M[0]), imm_log(0.2));

        imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(I[i]), imm_log(0.2));
        imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(I[i]), imm_log(0.2));

        if (i > 0) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_normal_state_super(M[i]), imm_log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_normal_state_super(M[i]), imm_log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i - 1]), imm_normal_state_super(M[i]), imm_log(0.2));

            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i - 1]), imm_mute_state_super(D[i]), imm_log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i - 1]), imm_mute_state_super(D[i]), imm_log(0.2));
        }

        if (i == ncore_nodes - 1) {
            imm_hmm_set_trans(hmm, imm_normal_state_super(M[i]), imm_normal_state_super(E), imm_log(0.2));
            imm_hmm_set_trans(hmm, imm_mute_state_super(D[i]), imm_normal_state_super(E), imm_log(0.2));
            imm_hmm_set_trans(hmm, imm_normal_state_super(I[i]), imm_normal_state_super(E), imm_log(0.2));
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
        imm_dp_task_setup(task, seq);
        struct imm_result const* r = imm_dp_viterbi(dp, task);
        loglik = imm_hmm_loglikelihood(hmm, seq, imm_result_path(r));
        cass_cond(is_valid(loglik) && !is_zero(loglik));
        seconds[i] = imm_result_seconds(r);
        imm_result_destroy(r);
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
