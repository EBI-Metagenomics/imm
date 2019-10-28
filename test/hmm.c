#include "cass/cass.h"
#include "imm.h"
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_mute_state(void);
void test_hmm_likelihood_two_mute_states(void);
void test_hmm_viterbi_no_state(void);
void test_hmm_viterbi_mute_cycle(void);
void test_hmm_viterbi_one_mute_state(void);
void test_hmm_viterbi_two_mute_states(void);
void test_hmm_viterbi_normal_states(void);
void test_hmm_viterbi_profile1(void);
void test_hmm_viterbi_profile2(void);
void test_hmm_viterbi_profile_delete(void);
void test_hmm_viterbi_global_profile(void);
void test_hmm_viterbi_table_states(void);

int main(void)
{
    test_hmm_state_id();
    test_hmm_del_get_state();
    test_hmm_set_trans();
    test_hmm_likelihood_single_state();
    test_hmm_likelihood_two_states();
    test_hmm_likelihood_mute_state();
    test_hmm_likelihood_two_mute_states();
    test_hmm_viterbi_no_state();
    test_hmm_viterbi_mute_cycle();
    test_hmm_viterbi_one_mute_state();
    test_hmm_viterbi_two_mute_states();
    /* test_hmm_viterbi_normal_states(); */
    /* test_hmm_viterbi_profile1(); */
    /* test_hmm_viterbi_profile2(); */
    /* test_hmm_viterbi_profile_delete(); */
    /* test_hmm_viterbi_global_profile(); */
    /* test_hmm_viterbi_table_states(); */
    return cass_status();
}

static inline int                     is_zero(double a) { return imm_lprob_is_zero(a); }
static inline const struct imm_state* cast_c(const void* s) { return imm_state_cast_c(s); }
static inline double                  zero(void) { return imm_lprob_zero(); }
static inline int                     is_valid(double a) { return imm_lprob_is_valid(a); }

void test_hmm_state_id(void)
{
    struct imm_abc*        abc = imm_abc_create("ACGT");
    struct imm_mute_state* state = imm_mute_state_create("State0", abc);
    struct imm_hmm*        hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, cast_c(state), log(1.0)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast_c(state), log(1.0)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_del_get_state(void)
{
    struct imm_abc*        abc = imm_abc_create("ACGT");
    struct imm_mute_state* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*        hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, cast_c(state0), log(0.5)) == 0);
    cass_cond(imm_hmm_add_state(hmm, cast_c(state1), log(0.5)) == 0);
    cass_cond(imm_hmm_add_state(hmm, NULL, log(0.5)) == 1);

    cass_cond(!is_valid(imm_hmm_get_trans(hmm, cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_get_trans(hmm, NULL, cast_c(state0))));
    cass_cond(!is_valid(imm_hmm_get_trans(hmm, NULL, NULL)));

    cass_cond(imm_hmm_del_state(hmm, cast_c(state0)) == 0);
    cass_cond(imm_hmm_del_state(hmm, cast_c(state1)) == 0);

    cass_cond(imm_hmm_del_state(hmm, cast_c(state0)) == 1);
    cass_cond(imm_hmm_del_state(hmm, cast_c(state1)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_set_trans(void)
{

    struct imm_abc*        abc = imm_abc_create("ACGT");
    struct imm_mute_state* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*        hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.5));

    cass_cond(imm_hmm_set_trans(hmm, cast_c(state0), NULL, log(0.5)) == 1);
    cass_cond(imm_hmm_set_trans(hmm, NULL, cast_c(state1), log(0.5)) == 1);
    cass_cond(imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.5)) == 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_single_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), zero()};

    struct imm_normal_state* state = imm_normal_state_create("State0", abc, lprobs);
    struct imm_hmm*          hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, cast_c(state), log(0.5));
    cass_cond(imm_hmm_normalize(hmm) == 1);

    struct imm_path* path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_close(imm_hmm_likelihood(hmm, "A", path), -1.386294361120);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 2);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "AG", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "H", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, NULL, 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    imm_path_add(path, cast_c(state), 1);
    cass_cond(is_zero(imm_hmm_likelihood(hmm, "AA", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, NULL, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "A", NULL)));
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, NULL, NULL)));

    cass_cond(imm_hmm_normalize(hmm) == 1);
    imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), zero());
    cass_cond(imm_hmm_normalize(hmm) == 1);
    imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), log(0.5));

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    imm_path_add(path, cast_c(state), 1);
    cass_close(imm_hmm_likelihood(hmm, "AA", path), -3.465735902800);
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize(hmm) == 0);
    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    imm_path_add(path, cast_c(state), 1);
    cass_close(imm_hmm_likelihood(hmm, "AA", path), -2.772588722240);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    double                   lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state* state0 = imm_normal_state_create("State0", abc, lprobs0);

    double                   lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_normal_state* state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(1.0));
    imm_hmm_add_state(hmm, cast_c(state1), zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_add(path, cast_c(state0), 1);
    cass_close(imm_hmm_likelihood(hmm, "A", path), -1.3862943611);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state0), 1);
    cass_cond(is_zero(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state1), 1);
    cass_cond(is_zero(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize(hmm) == 0);

    path = imm_path_create();
    imm_path_add(path, cast_c(state0), 1);
    cass_close(imm_hmm_likelihood(hmm, "G", path), -0.6931471806);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state0), 1);
    imm_path_add(path, cast_c(state1), 1);
    cass_close(imm_hmm_likelihood(hmm, "GT", path), -1.0986122887);
    imm_path_destroy(path);

    cass_cond(imm_normal_state_normalize(state1) == 0);
    path = imm_path_create();
    imm_path_add(path, cast_c(state0), 1);
    imm_path_add(path, cast_c(state1), 1);
    cass_close(imm_hmm_likelihood(hmm, "GT", path), -1.9095425049);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_mute_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* state = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, cast_c(state), log(1.0));

    imm_hmm_set_trans(hmm, cast_c(state), cast_c(state), log(0.1));

    struct imm_path* path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 0);
    cass_close(imm_hmm_likelihood(hmm, "", path), 0.0);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_add(path, cast_c(state), 1);
    imm_path_add(path, cast_c(state), 1);
    cass_cond(!is_valid(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_two_mute_states(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* S0 = imm_mute_state_create("S0", abc);
    struct imm_mute_state* S1 = imm_mute_state_create("S1", abc);

    imm_hmm_add_state(hmm, cast_c(S0), 0.0);
    imm_hmm_add_state(hmm, cast_c(S1), zero());

    imm_hmm_set_trans(hmm, cast_c(S0), cast_c(S1), 0.0);

    struct imm_path* path = imm_path_create();
    imm_path_add(path, cast_c(S0), 0);
    imm_path_add(path, cast_c(S1), 0);
    cass_close(imm_hmm_likelihood(hmm, "", path), 0.0);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S0);
    imm_mute_state_destroy(S1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_no_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", NULL, NULL)));

    imm_hmm_destroy(hmm);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_one_mute_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* state = imm_mute_state_create("state", abc);

    imm_hmm_add_state(hmm, cast_c(state), log(0.5));

    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state), NULL), log(0.5));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "X", cast_c(state), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "C", cast_c(state), NULL)));

    imm_hmm_set_start_lprob(hmm, cast_c(state), imm_lprob_zero());

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "X", cast_c(state), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "C", cast_c(state), NULL)));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_two_mute_states(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* state0 = imm_mute_state_create("state", abc);
    struct imm_mute_state* state1 = imm_mute_state_create("state", abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));
    imm_hmm_add_state(hmm, cast_c(state1), log(0.1));

    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL), log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL), log(0.1));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "X", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "X", cast_c(state1), NULL)));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "C", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "C", cast_c(state1), NULL)));

    imm_hmm_set_start_lprob(hmm, cast_c(state1), imm_lprob_zero());

    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL), log(0.5));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.1));

    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL), log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL), log(0.5) + log(0.1));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_mute_cycle(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* state0 = imm_mute_state_create("State0", abc);

    imm_hmm_add_state(hmm, cast_c(state0), log(0.5));

    struct imm_mute_state* state1 = imm_mute_state_create("State1", abc);
    imm_hmm_add_state(hmm, cast_c(state1), imm_lprob_zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state0), log(0.2));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_normal_states(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    double                   lprobs0[] = {log(0.25), log(0.25), log(0.5), zero()};
    struct imm_normal_state* state0 = imm_normal_state_create("State0", abc, lprobs0);

    double                   lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_normal_state* state1 = imm_normal_state_create("State1", abc, lprobs1);

    imm_hmm_add_state(hmm, cast_c(state0), log(1.0));
    imm_hmm_add_state(hmm, cast_c(state1), zero());

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), log(1.0));

    cass_cond(imm_normal_state_normalize(state0) == 0);
    cass_cond(imm_normal_state_normalize(state1) == 0);

    imm_hmm_normalize(hmm);

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));

    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(state0), NULL), -1.386294361120);

    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state1), NULL)));

    cass_close(imm_hmm_viterbi(hmm, "AG", cast_c(state0), NULL), -3.178053830348);

    cass_close(imm_hmm_viterbi(hmm, "AG", cast_c(state1), NULL), -3.295836866004);

    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AGT", cast_c(state0), NULL)));

    cass_close(imm_hmm_viterbi(hmm, "AGT", cast_c(state1), NULL), -4.106767082221);

    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AGTC", cast_c(state0), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "AGTC", cast_c(state1), NULL), -6.303991659557);

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), zero());
    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), zero());
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state0), zero());
    imm_hmm_set_trans(hmm, cast_c(state1), cast_c(state1), zero());

    imm_hmm_set_start_lprob(hmm, cast_c(state0), zero());
    imm_hmm_set_start_lprob(hmm, cast_c(state1), zero());

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state0), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state1), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(state0), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(state1), NULL)));

    imm_hmm_set_start_lprob(hmm, cast_c(state0), 0.0);

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(state0), NULL), log(0.25));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state1), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(state0), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(state1), NULL)));

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state0), log(0.9));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(state0), NULL), log(0.25));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state1), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(state0), NULL), 2 * log(0.25) + log(0.9));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(state1), NULL)));

    imm_hmm_set_trans(hmm, cast_c(state0), cast_c(state1), log(0.2));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state0), NULL)));
    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(state1), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(state0), NULL), log(0.25));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(state1), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(state0), NULL), 2 * log(0.25) + log(0.9));

    double desired = log(0.25) + log(0.5 / 2.25) + log(0.2);
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(state1), NULL), desired);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile1(void)
{
    struct imm_abc* abc = imm_abc_create("AB");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* start = imm_mute_state_create("START", abc);
    struct imm_mute_state* D0 = imm_mute_state_create("D0", abc);
    struct imm_mute_state* end = imm_mute_state_create("END", abc);

    double                   M0_lprobs[] = {log(0.4), log(0.2)};
    struct imm_normal_state* M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double                   I0_lprobs[] = {log(0.5), log(0.5)};
    struct imm_normal_state* I0 = imm_normal_state_create("I0", abc, I0_lprobs);

    imm_hmm_add_state(hmm, cast_c(start), 0.0);
    imm_hmm_add_state(hmm, cast_c(D0), zero());
    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(D0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(D0), cast_c(end), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(end), log(0.8));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(end), log(1.0));

    cass_close(imm_hmm_viterbi(hmm, "", cast_c(end), NULL), log(0.1) + log(1.0));
    cass_close(imm_hmm_viterbi(hmm, "", cast_c(D0), NULL), log(0.1));
    cass_close(imm_hmm_viterbi(hmm, "", cast_c(start), NULL), log(1.0));

    cass_cond(!is_valid(imm_hmm_viterbi(hmm, "", cast_c(M0), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(start), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(D0), NULL)));
    cass_cond(is_zero(imm_hmm_viterbi(hmm, "A", cast_c(I0), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(M0), NULL), log(0.5) + log(0.4));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(end), NULL), log(0.5) + log(0.4) + log(0.8));
    cass_close(imm_hmm_viterbi(hmm, "B", cast_c(M0), NULL), log(0.5) + log(0.2));
    cass_close(imm_hmm_viterbi(hmm, "B", cast_c(end), NULL), log(0.5) + log(0.2) + log(0.8));

    cass_cond(is_zero(imm_hmm_viterbi(hmm, "AA", cast_c(M0), NULL)));

    double desired = log(0.5) + log(0.4) + log(0.1) + log(0.5);
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(end), NULL), desired);

    desired = log(0.5) + log(0.4) + log(0.1) + log(0.5);
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(end), NULL), desired);

    desired = log(0.5) + log(0.4) + log(0.1) + log(0.2) + 2 * log(0.5);
    cass_close(imm_hmm_viterbi(hmm, "AAB", cast_c(end), NULL), desired);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(D0);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile2(void)
{
    struct imm_abc* abc = imm_abc_create("ABCD");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state* start = imm_mute_state_create("START", abc);

    double ins_lprobs[] = {log(0.1), log(0.1), log(0.1), log(0.7)};

    double M0_lprobs[] = {log(0.4), zero(), log(0.6), zero()};
    double M1_lprobs[] = {log(0.6), zero(), log(0.4), zero()};
    double M2_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05)};

    struct imm_normal_state* M0 = imm_normal_state_create("M0", abc, M0_lprobs);
    struct imm_normal_state* I0 = imm_normal_state_create("I0", abc, ins_lprobs);

    struct imm_mute_state*   D1 = imm_mute_state_create("D1", abc);
    struct imm_normal_state* M1 = imm_normal_state_create("M1", abc, M1_lprobs);
    struct imm_normal_state* I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_mute_state*   D2 = imm_mute_state_create("D2", abc);
    struct imm_normal_state* M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_mute_state* end = imm_mute_state_create("END", abc);

    imm_hmm_add_state(hmm, cast_c(start), 0.0);

    imm_hmm_add_state(hmm, cast_c(M0), zero());
    imm_hmm_add_state(hmm, cast_c(I0), zero());

    imm_hmm_add_state(hmm, cast_c(D1), zero());
    imm_hmm_add_state(hmm, cast_c(M1), zero());
    imm_hmm_add_state(hmm, cast_c(I1), zero());

    imm_hmm_add_state(hmm, cast_c(D2), zero());
    imm_hmm_add_state(hmm, cast_c(M2), zero());

    imm_hmm_add_state(hmm, cast_c(end), zero());

    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M0), 0.0);
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M1), 0.0);
    imm_hmm_set_trans(hmm, cast_c(start), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M1), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(end), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(M2), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(end), 0.0);
    imm_hmm_set_trans(hmm, cast_c(M2), cast_c(end), 0.0);

    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(I0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(M0), cast_c(D1), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(I0), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I0), cast_c(M1), log(0.5));

    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(D2), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(M1), cast_c(I1), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(I1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(I1), cast_c(M2), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(D2), log(0.3));
    imm_hmm_set_trans(hmm, cast_c(D1), cast_c(M2), log(0.7));

    imm_hmm_set_trans(hmm, cast_c(D2), cast_c(end), log(1.0));

    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(M2), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "B", cast_c(M2), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(M2), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "D", cast_c(M2), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(end), NULL), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "B", cast_c(end), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(end), NULL), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "D", cast_c(end), NULL), log(0.05));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(M1), NULL), log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(M1), NULL), log(0.4));

    cass_close(imm_hmm_viterbi(hmm, "CA", cast_c(end), NULL), 2 * log(0.6));
    cass_close(imm_hmm_viterbi(hmm, "CD", cast_c(I0), NULL), log(0.6) + log(0.2) + log(0.7));

    double desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6);
    cass_close(imm_hmm_viterbi(hmm, "CDDDA", cast_c(end), NULL), desired);

    desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.05);
    cass_close(imm_hmm_viterbi(hmm, "CDDDAB", cast_c(end), NULL), desired);

    desired = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.2) +
              log(0.1) + log(0.5) + log(0.05);
    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", cast_c(M2), NULL), desired);

    desired = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6);
    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", cast_c(M1), NULL), desired);

    desired = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) + log(0.6);
    cass_close(imm_hmm_viterbi(hmm, "CDDDABA", cast_c(end), NULL), desired);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(D1);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(M2);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile_delete(void)
{
    struct imm_abc* abc = imm_abc_create("AB");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    double                   N0_lprobs[] = {log(0.5), zero()};
    struct imm_normal_state* N0 = imm_normal_state_create("N0", abc, N0_lprobs);

    struct imm_mute_state* M = imm_mute_state_create("M", abc);

    double                   N1_lprobs[] = {log(0.5), zero()};
    struct imm_normal_state* N1 = imm_normal_state_create("N1", abc, N1_lprobs);

    double                   N2_lprobs[] = {zero(), log(0.5)};
    struct imm_normal_state* N2 = imm_normal_state_create("N2", abc, N2_lprobs);

    imm_hmm_add_state(hmm, cast_c(N2), zero());
    imm_hmm_add_state(hmm, cast_c(N1), zero());
    imm_hmm_add_state(hmm, cast_c(M), zero());
    imm_hmm_add_state(hmm, cast_c(N0), 0);

    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(N1), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(M), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(N1), cast_c(N2), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(M), cast_c(N2), log(0.5));

    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(N0), NULL), log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(M), NULL), 2 * log(0.5));
    cass_close(imm_hmm_viterbi(hmm, "AB", cast_c(N2), NULL), 4 * log(0.5));

    cass_cond(imm_hmm_del_state(hmm, cast_c(N2)) == 0);
    cass_close(imm_hmm_viterbi(hmm, "A", cast_c(M), NULL), 2 * log(0.5));

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(N0);
    imm_mute_state_destroy(M);
    imm_normal_state_destroy(N1);
    imm_normal_state_destroy(N2);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_global_profile(void)
{
    struct imm_abc* abc = imm_abc_create("ABCZ");
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

    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(start), NULL), zero());
    cass_close(imm_hmm_viterbi(hmm, "C", cast_c(B), NULL), 0);
    cass_close(imm_hmm_viterbi(hmm, "CC", cast_c(B), NULL), 0);
    cass_close(imm_hmm_viterbi(hmm, "CCC", cast_c(B), NULL), 0);
    cass_close(imm_hmm_viterbi(hmm, "CCA", cast_c(B), NULL), log(0.01));
    cass_close(imm_hmm_viterbi(hmm, "CCA", cast_c(M0), NULL), log(0.9));
    cass_close(imm_hmm_viterbi(hmm, "CCAB", cast_c(M1), NULL), 2 * log(0.9));
    cass_close(imm_hmm_viterbi(hmm, "CCAB", cast_c(I0), NULL), log(0.9 * 0.5 * 0.1));
    cass_close(imm_hmm_viterbi(hmm, "CCABB", cast_c(I0), NULL), log(0.9) + 2 * (log(0.05)));

    double desired = log(0.9) + log(0.5) + log(0.1) + log(0.5) + log(0.01);
    cass_close(imm_hmm_viterbi(hmm, "CCABA", cast_c(M1), NULL), desired);
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(D1), NULL), log(0.01) + log(0.9));
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(D2), NULL), log(0.01) + log(0.9));
    cass_close(imm_hmm_viterbi(hmm, "AA", cast_c(E), NULL), log(0.01) + log(0.9));

    desired = log(0.01) + log(0.9) + log(0.5);
    cass_close(imm_hmm_viterbi(hmm, "AAB", cast_c(M2), NULL), desired);

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

void test_hmm_viterbi_table_states(void)
{
    struct imm_abc* abc = imm_abc_create("ACGTX");
    struct imm_hmm* hmm = imm_hmm_create(abc);

    struct imm_mute_state*  S = imm_mute_state_create("S", abc);
    struct imm_table_state* T = imm_table_state_create("T", abc);
    imm_table_state_add(T, "", log(0.5));
    imm_table_state_add(T, "A", log(0.1));
    imm_table_state_add(T, "TAT", log(0.2));

    struct imm_mute_state* D = imm_mute_state_create("D", abc);

    double                   N0_lprobs[] = {zero(), log(0.5), log(0.5), zero(), zero()};
    struct imm_normal_state* N0 = imm_normal_state_create("N0", abc, N0_lprobs);

    double N1_lprobs[] = {log(0.25), log(0.25), log(0.25), log(0.25), zero()};
    struct imm_normal_state* N1 = imm_normal_state_create("N1", abc, N1_lprobs);

    struct imm_mute_state* E = imm_mute_state_create("E", abc);

    double                   Z_lprobs[] = {zero(), zero(), zero(), zero(), log(1.0)};
    struct imm_normal_state* Z = imm_normal_state_create("Z", abc, Z_lprobs);

    imm_hmm_add_state(hmm, cast_c(S), log(1.0));
    imm_hmm_add_state(hmm, cast_c(T), zero());
    imm_hmm_add_state(hmm, cast_c(N0), zero());
    imm_hmm_add_state(hmm, cast_c(D), zero());
    imm_hmm_add_state(hmm, cast_c(N1), zero());
    imm_hmm_add_state(hmm, cast_c(E), zero());
    imm_hmm_add_state(hmm, cast_c(Z), zero());

    cass_cond(imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), imm_lprob_invalid()) == 1);
    cass_cond(imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), imm_lprob_invalid()) == 1);

    imm_hmm_set_trans(hmm, cast_c(S), cast_c(T), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(T), cast_c(D), log(0.1));
    imm_hmm_set_trans(hmm, cast_c(T), cast_c(N0), log(0.2));
    imm_hmm_set_trans(hmm, cast_c(D), cast_c(N1), log(0.3));
    imm_hmm_set_trans(hmm, cast_c(N0), cast_c(N1), log(0.4));
    imm_hmm_set_trans(hmm, cast_c(N1), cast_c(E), log(1.0));
    imm_hmm_set_trans(hmm, cast_c(E), cast_c(Z), log(0.5));
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(Z), log(2.0));
    imm_hmm_set_trans(hmm, cast_c(Z), cast_c(T), log(0.6));

    cass_cond(is_zero(imm_hmm_viterbi(hmm, "TATX", cast_c(E), NULL)));
    cass_close(imm_hmm_viterbi(hmm, "TATA", cast_c(N1), NULL), -6.502290170873972);
    cass_close(imm_hmm_viterbi(hmm, "TATA", cast_c(E), NULL), -6.502290170873972);
    cass_close(imm_hmm_viterbi(hmm, "TATTX", cast_c(Z), NULL), -7.195437351433918);
    cass_close(imm_hmm_viterbi(hmm, "TATTXX", cast_c(Z), NULL), -6.502290170873972);
    cass_close(imm_hmm_viterbi(hmm, "CAXCA", cast_c(E), NULL), -11.800607537422009);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S);
    imm_table_state_destroy(T);
    imm_mute_state_destroy(D);
    imm_normal_state_destroy(N0);
    imm_normal_state_destroy(N1);
    imm_mute_state_destroy(E);
    imm_normal_state_destroy(Z);
    imm_abc_destroy(abc);
}
