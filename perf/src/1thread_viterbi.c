#include "cass/cass.h"
#include "imm/imm.h"
#include "stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NSAMPLES 100

imm_float perf_1thread_viterbi(double* seconds, uint16_t ncore_nodes, uint16_t seq_100length);

int main(void)
{
    imm_float const scores[] = {
        -394.311890,    -3943.085205,   -7886.054199,   -11829.023438,  -15771.992188,
        -19714.960938,  -23657.929688,  -27600.898438,  -31543.867188,  -35486.835938,
        -39429.804688,  -1681.885620,   -16818.085938,  -33636.054688,  -50454.023438,
        -67271.992188,  -84089.960938,  -100907.929688, -117725.898438, -134543.875000,
        -151361.843750, -168179.812500, -3291.288330,   -32911.835938,  -65823.554688,
        -98735.273438,  -131647.000000, -164558.718750, -197470.437500, -230382.156250,
        -263305.031250, -296536.281250, -329767.531250};

    printf("ncore_nodes,seq_length,median,std_err_mean,score,perf_name\n");

    uint16_t ncore_nodes[] = {100, 500, 1000};
    uint16_t seq_100len[] = {1, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
    for (unsigned i = 0; i < 3; ++i) {
        for (unsigned j = 0; j < IMM_ARRAY_SIZE(seq_100len); ++j) {
            uint16_t len = seq_100len[j];
            double   seconds[NSAMPLES] = {0.};
            imm_float   score = perf_1thread_viterbi(seconds, ncore_nodes[i], len);
            cass_close(score, scores[i * IMM_ARRAY_SIZE(seq_100len) + j]);
            struct stats stats = compute_stats(seconds, NSAMPLES);
            char const   fmt[] = "%d,%d,%.6f,%.6f,%.6f,1thread_viterbi\n";
            printf(fmt, ncore_nodes[i], len * 100, stats.median, stats.sem, score);
        }
    }
    return 0;
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int    is_valid(imm_float a) { return imm_lprob_is_valid(a); }
static inline int    is_zero(imm_float a) { return imm_lprob_is_zero(a); }
static inline char*  fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

imm_float perf_1thread_viterbi(imm_float* seconds, uint16_t ncore_nodes, uint16_t seq_100length)
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
    char* str = malloc(sizeof(*str) * (100 * seq_100length + 1));
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

    imm_float score = 0.0;
    for (unsigned i = 0; i < NSAMPLES; ++i) {
        struct imm_results const* results = imm_dp_viterbi(dp, seq, 0);
        cass_cond(imm_results_size(results) == 1);
        struct imm_result const* r = imm_results_get(results, 0);
        score = imm_result_loglik(r);
        cass_cond(is_valid(score) && !is_zero(score));
        seconds[i] = imm_result_seconds(r);
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
    free((void*)str);

    return score;
}
