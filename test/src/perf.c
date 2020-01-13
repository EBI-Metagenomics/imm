#include "cass.h"
#include "imm/imm.h"
#include <stdlib.h>

void test_perf_viterbi(void);

int main(void)
{
    test_perf_viterbi();
    return cass_status();
}

static inline int                     is_zero(double a) { return imm_lprob_is_zero(a); }
static inline struct imm_state const* cast_c(void const* s) { return imm_state_cast_c(s); }
static inline double                  zero(void) { return imm_lprob_zero(); }
static inline int                     is_valid(double a) { return imm_lprob_is_valid(a); }

void test_perf_viterbi(void)
{
    struct imm_abc* abc = imm_abc_create("ABCZ", '*');
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* start = imm_mute_state_create("START", abc);

    double                   B_lprobs[] = {log(0.01), log(0.01), log(1.0), zero()};
    struct imm_normal_state* B = imm_normal_state_create("B", abc, B_lprobs);

    double                   M0_lprobs[] = {log(0.9), log(0.01), log(0.01), zero()};
    struct imm_normal_state* M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double                   M1_lprobs[] = {log(0.01), log(0.9), zero(), zero()};
    struct imm_normal_state* M1 = imm_normal_state_create("M1", abc, M1_lprobs);

    double                   M2_lprobs[] = {log(0.5), log(0.5), zero(), zero()};
    struct imm_normal_state* M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_mute_state* E = imm_mute_state_create("E", abc);
    struct imm_mute_state* end = imm_mute_state_create("END", abc);

    double                   Z_lprobs[] = {zero(), zero(), zero(), log(1.0)};
    struct imm_normal_state* Z = imm_normal_state_create("Z", abc, Z_lprobs);

    double                   ins_lprobs[] = {log(0.1), log(0.1), log(0.1), zero()};
    struct imm_normal_state* I0 = imm_normal_state_create("I0", abc, ins_lprobs);
    struct imm_normal_state* I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_mute_state* D1 = imm_mute_state_create("D1", abc);
    struct imm_mute_state* D2 = imm_mute_state_create("D2", abc);

    imm_hmm_add_state(hmm, cast_c(start), log(1.0));
    imm_hmm_add_state(hmm, cast_c(B), zero());
    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(M1), zero());
    imm_hmm_add_state(hmm, cast_c(M2), zero());
    imm_hmm_add_state(hmm, cast_c(D1), zero());
    imm_hmm_add_state(hmm, cast_c(D2), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());
    imm_hmm_add_state(hmm, cast_c(I1), zero());
    imm_hmm_add_state(hmm, cast_c(E), zero());
    imm_hmm_add_state(hmm, cast_c(Z), zero());
    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(B), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(B), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M0), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M1), 0);
    imm_hmm_set_trans(hmm, cast_c(B), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M1), 0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(M2), cast_c(E), 0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(E), 0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(E), 0);

    imm_hmm_set_trans(hmm, cast_c(E), cast_c(end), 0);

    imm_hmm_set_trans(hmm, cast_c(E), cast_c(Z), 0);
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(Z), 0);
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(B), 0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(D1), 0);
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(D2), 0);
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(M2), 0);
    imm_hmm_set_trans(hmm, cast_c(D2), cast_c(E), 0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(M1), log(0.5));

    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(M2), log(0.5));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "C", cast_c(start), path)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "C", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, "C", path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CC", cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, "CC", path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCC", cast_c(B), path), 0);
    cass_close(imm_hmm_likelihood(hmm, "CCC", path), 0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCA", cast_c(B), path), log(0.01));
    cass_close(imm_hmm_likelihood(hmm, "CCA", path), log(0.01));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCA", cast_c(M0), path), log(0.9));
    cass_close(imm_hmm_likelihood(hmm, "CCA", path), log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCAB", cast_c(M1), path), 2 * log(0.9));
    cass_close(imm_hmm_likelihood(hmm, "CCAB", path), 2 * log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCAB", cast_c(I0), path), log(0.9 * 0.5 * 0.1));
    cass_close(imm_hmm_likelihood(hmm, "CCAB", path), log(0.9 * 0.5 * 0.1));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "CCABB", cast_c(I0), path), log(0.9) + 2 * (log(0.05)));
    cass_close(imm_hmm_likelihood(hmm, "CCABB", path), log(0.9) + 2 * (log(0.05)));
    imm_path_destroy(path);

    path = imm_path_create();
    double desired = log(0.9) + log(0.5) + log(0.1) + log(0.5) + log(0.01);
    cass_close(imm_hmm_viterbi(hmm, "CCABA", cast_c(M1), path), desired);
    cass_close(imm_hmm_likelihood(hmm, "CCABA", path), desired);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(D1), path), log(0.01) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, "AA", path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(D2), path), log(0.01) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, "AA", path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(E), path), log(0.01) + log(0.9));
    cass_close(imm_hmm_likelihood(hmm, "AA", path), log(0.01) + log(0.9));
    imm_path_destroy(path);

    path = imm_path_create();
    desired = log(0.01) + log(0.9) + log(0.5);
    cass_close(imm_hmm_viterbi(hmm, "AAB", cast_c(M2), path), desired);
    cass_close(imm_hmm_likelihood(hmm, "AAB", path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(B);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(M2);
    imm_normal_state_destroy(I0);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(Z);
    imm_mute_state_destroy(E);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
}
