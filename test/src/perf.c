#include "cass.h"
#include "elapsed.h"
#include "imm/imm.h"
#include <stdio.h>
#include <stdlib.h>

void test_perf_viterbi(void);

int main(void)
{
    test_perf_viterbi();
    return cass_status();
}

static inline double                  zero(void) { return imm_lprob_zero(); }
static inline int                     is_valid(double a) { return imm_lprob_is_valid(a); }
static inline int                     is_zero(double a) { return imm_lprob_is_zero(a); }
static inline struct imm_state const* cast_c(void const* s) { return imm_state_cast_c(s); }
static inline char*                   fmt_name(char* restrict buffer, char const* name, int i)
{
    sprintf(buffer, "%s%d", name, i);
    return buffer;
}

void test_perf_viterbi(void)
{
    int ncore_nodes = 1000;

    struct imm_abc* abc = imm_abc_create("BMIEJ", '*');
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* start = imm_mute_state_create("START", abc);
    imm_hmm_add_state(hmm, cast_c(start), log(1.0));

    struct imm_mute_state* end = imm_mute_state_create("END", abc);
    imm_hmm_add_state(hmm, cast_c(end), zero());

    double B_lprobs[] = {log(1.0), zero(), zero(), zero(), zero()};
    double E_lprobs[] = {zero(), zero(), zero(), log(1.0), zero()};
    double J_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    double M_lprobs[] = {zero(), log(1.0), zero(), zero(), zero()};
    double I_lprobs[] = {zero(), zero(), log(1.0), zero(), zero()};

    struct imm_normal_state* B = imm_normal_state_create("B", abc, B_lprobs);
    imm_hmm_add_state(hmm, cast_c(B), zero());
    struct imm_normal_state* E = imm_normal_state_create("E", abc, E_lprobs);
    imm_hmm_add_state(hmm, cast_c(E), zero());
    struct imm_normal_state* J = imm_normal_state_create("J", abc, J_lprobs);
    imm_hmm_add_state(hmm, cast_c(J), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(E), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(J), cast_c(J), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(J), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(J), cast_c(B), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(end), log(0.2));

    struct imm_normal_state* M[ncore_nodes];
    struct imm_normal_state* I[ncore_nodes];
    struct imm_mute_state*   D[ncore_nodes];

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
    elapsed_start(elapsed);
    /* imm_mstate_sort: 0.0024150000 seconds */
    /* imm_dp_create: 0.0331740000 seconds */
    /* imm_dp_viterbi: 3.6272990000 seconds */
    /* Score: -3247.8457072921 */
    /* Elapsed: 3.6685810000 seconds */
    double score = imm_hmm_viterbi(hmm, "BMIIMIIMMIMMMIMEJBMIIMIIMMIMMMIME", cast_c(end), path);
    printf("Score: %.10f\n", score);
    cass_cond(is_valid(score) && !is_zero(score));
    cass_close(score, -3247.8457072921);
    elapsed_end(elapsed);
    imm_path_destroy(path);

    printf("Elapsed: %.10f seconds\n", elapsed_seconds(elapsed));

    elapsed_destroy(elapsed);
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
}
