#include "imm.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_mute_state(void);
void test_hmm_viterbi_no_state(void);
void test_hmm_viterbi_mute_states(void);
void test_hmm_viterbi_normal_states(void);
void test_hmm_viterbi_profile1(void);
void test_hmm_viterbi_profile2(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hmm_state_id);
    RUN_TEST(test_hmm_del_get_state);
    RUN_TEST(test_hmm_set_trans);
    RUN_TEST(test_hmm_likelihood_single_state);
    RUN_TEST(test_hmm_likelihood_two_states);
    RUN_TEST(test_hmm_likelihood_mute_state);
    RUN_TEST(test_hmm_viterbi_no_state);
    RUN_TEST(test_hmm_viterbi_mute_states);
    RUN_TEST(test_hmm_viterbi_normal_states);
    RUN_TEST(test_hmm_viterbi_profile1);
    RUN_TEST(test_hmm_viterbi_profile2);
    return UNITY_END();
}

void test_hmm_state_id(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_mute_state *state = imm_mute_state_create("State0", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    int state_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state), log(1.0));
    TEST_ASSERT_EQUAL_INT(0, state_id);

    state_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state), log(1.0));
    TEST_ASSERT_EQUAL_INT(1, state_id);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_del_get_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_mute_state *state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state *state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    int state_id0 = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state0), log(0.5));
    int state_id1 = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state1), log(0.5));
    TEST_ASSERT_EQUAL_INT(IMM_INVALID_STATE_ID, imm_hmm_add_state(hmm, NULL, log(0.5)));

    TEST_ASSERT_EQUAL_INT(0, state_id0);
    TEST_ASSERT_EQUAL_INT(1, state_id1);

    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_get_trans(hmm, 0, 5));
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_get_trans(hmm, 5, 0));
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_get_trans(hmm, 5, 5));

    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id0));
    imm_hmm_del_state(hmm, state_id0);
    TEST_ASSERT_NULL(imm_hmm_get_state(hmm, state_id0));

    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id1));
    imm_hmm_del_state(hmm, state_id1);
    TEST_ASSERT_NULL(imm_hmm_get_state(hmm, state_id1));

    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_del_state(hmm, state_id0));
    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_del_state(hmm, state_id1));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_set_trans(void)
{

    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_mute_state *state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state *state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    const struct imm_state *s0 = imm_mute_state_cast_c(state0);
    const struct imm_state *s1 = imm_mute_state_cast_c(state1);

    int state_id0 = imm_hmm_add_state(hmm, s0, log(0.5));
    int state_id1 = imm_hmm_add_state(hmm, s1, log(0.5));

    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id0));
    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id1));

    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_set_trans(hmm, state_id0, 5, log(0.5)));
    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_set_trans(hmm, 5, state_id1, log(0.5)));
    TEST_ASSERT_EQUAL_INT(0, imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.5)));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_single_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};

    struct imm_normal_state *state = imm_normal_state_create("State0", abc, lprobs);
    struct imm_hmm *hmm = imm_hmm_create(abc);

    int state_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(state), log(0.5));
    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_normalize(hmm));

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-1.386294361120, imm_hmm_likelihood(hmm, "A", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 2);
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, "A", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, "AG", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_likelihood(hmm, "H", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, 5, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, "A", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_likelihood(hmm, "AA", path));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, NULL, path));
    imm_path_destroy(&path);

    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, "A", NULL));
    TEST_ASSERT_DOUBLE_IS_NAN(imm_hmm_likelihood(hmm, NULL, NULL));

    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_normalize(hmm));
    imm_hmm_set_trans(hmm, state_id, state_id, -INFINITY);
    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_normalize(hmm));
    imm_hmm_set_trans(hmm, state_id, state_id, log(0.5));

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-3.465735902800, imm_hmm_likelihood(hmm, "AA", path));
    imm_path_destroy(&path);

    TEST_ASSERT_EQUAL_INT(0, imm_hmm_normalize(hmm));
    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-2.772588722240, imm_hmm_likelihood(hmm, "AA", path));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    double lprobs0[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_normal_state *state0 = imm_normal_state_create("State0", abc, lprobs0);

    double lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_normal_state *state1 = imm_normal_state_create("State1", abc, lprobs1);

    int state_id0 = imm_hmm_add_state(hmm, imm_normal_state_cast_c(state0), log(1.0));
    int state_id1 = imm_hmm_add_state(hmm, imm_normal_state_cast_c(state1), -INFINITY);

    imm_hmm_set_trans(hmm, state_id0, state_id0, log(0.1));
    imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.2));
    imm_hmm_set_trans(hmm, state_id1, state_id1, log(1.0));

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0.25, exp(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id1, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    TEST_ASSERT_EQUAL_INT(0, imm_hmm_normalize(hmm));

    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0.5, exp(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    imm_path_add(&path, state_id1, 1);
    TEST_ASSERT_EQUAL_DOUBLE(1.0 / 3.0, exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    TEST_ASSERT_EQUAL_INT(0, imm_normal_state_normalize(state1));
    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    imm_path_add(&path, state_id1, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0.14814814814815, exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_likelihood_mute_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_mute_state *state = imm_mute_state_create("State0", abc);

    int state_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state), log(1.0));

    imm_hmm_set_trans(hmm, state_id, state_id, log(0.1));

    struct imm_path *path = NULL;
    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "A", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "T", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "G", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

/* printf("\n"); */
/* printf("score: %.12f\n", imm_hmm_viterbi(hmm, "", state_id0)); */
/* printf("score: %.12f\n", imm_hmm_viterbi(hmm, "", state_id1)); */

void test_hmm_viterbi_no_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", 0));

    imm_hmm_destroy(hmm);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_mute_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_mute_state *state0 = imm_mute_state_create("State0", abc);

    int state_id0 = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state0), log(0.5));

    imm_hmm_set_trans(hmm, state_id0, state_id0, log(0.1));

    TEST_ASSERT_EQUAL_DOUBLE(-0.693147180560, imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id0));

    struct imm_mute_state *state1 = imm_mute_state_create("State1", abc);
    int state_id1 = imm_hmm_add_state(hmm, imm_mute_state_cast_c(state1), log(0.2));

    imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.2));

    TEST_ASSERT_EQUAL_DOUBLE(-0.693147180560, imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(-1.609437912434, imm_hmm_viterbi(hmm, "", state_id1));

    TEST_ASSERT_EQUAL_INT(0, imm_hmm_set_start_lprob(hmm, state_id1, -INFINITY));

    TEST_ASSERT_EQUAL_DOUBLE(-0.693147180560, imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(-2.302585092994, imm_hmm_viterbi(hmm, "", state_id1));

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_normal_states(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    double lprobs0[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_normal_state *state0 = imm_normal_state_create("State0", abc, lprobs0);

    double lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_normal_state *state1 = imm_normal_state_create("State1", abc, lprobs1);

    int state_id0 = imm_hmm_add_state(hmm, imm_normal_state_cast_c(state0), log(1.0));
    int state_id1 = imm_hmm_add_state(hmm, imm_normal_state_cast_c(state1), -INFINITY);

    imm_hmm_set_trans(hmm, state_id0, state_id0, log(0.1));
    imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.2));
    imm_hmm_set_trans(hmm, state_id1, state_id1, log(1.0));

    TEST_ASSERT_EQUAL_INT(0, imm_normal_state_normalize(state0));
    TEST_ASSERT_EQUAL_INT(0, imm_normal_state_normalize(state1));

    imm_hmm_normalize(hmm);

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id1));

    TEST_ASSERT_EQUAL_DOUBLE(-1.386294361120, imm_hmm_viterbi(hmm, "A", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id1));

    TEST_ASSERT_EQUAL_DOUBLE(-3.178053830348, imm_hmm_viterbi(hmm, "AG", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(-3.295836866004, imm_hmm_viterbi(hmm, "AG", state_id1));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AGT", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(-4.106767082221, imm_hmm_viterbi(hmm, "AGT", state_id1));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AGTC", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(-6.303991659557, imm_hmm_viterbi(hmm, "AGTC", state_id1));

    imm_hmm_set_trans(hmm, state_id0, state_id0, -INFINITY);
    imm_hmm_set_trans(hmm, state_id0, state_id1, -INFINITY);
    imm_hmm_set_trans(hmm, state_id1, state_id0, -INFINITY);
    imm_hmm_set_trans(hmm, state_id1, state_id1, -INFINITY);

    imm_hmm_set_start_lprob(hmm, state_id0, -INFINITY);
    imm_hmm_set_start_lprob(hmm, state_id1, -INFINITY);

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", state_id1));

    imm_hmm_set_start_lprob(hmm, state_id0, 0.0);

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), imm_hmm_viterbi(hmm, "A", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id1));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", state_id1));

    imm_hmm_set_trans(hmm, state_id0, state_id0, log(0.9));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), imm_hmm_viterbi(hmm, "A", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id1));
    TEST_ASSERT_EQUAL_DOUBLE(2 * log(0.25) + log(0.9), imm_hmm_viterbi(hmm, "AA", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", state_id1));

    imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.2));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", state_id1));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25), imm_hmm_viterbi(hmm, "A", state_id0));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", state_id1));
    TEST_ASSERT_EQUAL_DOUBLE(2 * log(0.25) + log(0.9), imm_hmm_viterbi(hmm, "AA", state_id0));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.25) + log(0.5 / 2.25) + log(0.2),
                             imm_hmm_viterbi(hmm, "AA", state_id1));

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_viterbi_profile1(void)
{
    struct imm_abc *abc = imm_abc_create("AB");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_mute_state *start = imm_mute_state_create("START", abc);
    struct imm_mute_state *D0 = imm_mute_state_create("D0", abc);
    struct imm_mute_state *end = imm_mute_state_create("END", abc);

    double M0_lprobs[] = {log(0.4), log(0.2)};
    struct imm_normal_state *M0 = imm_normal_state_create("M0", abc, M0_lprobs);

    double I0_lprobs[] = {log(0.5), log(0.5)};
    struct imm_normal_state *I0 = imm_normal_state_create("I0", abc, I0_lprobs);

    int start_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(start), 0.0);
    int D0_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(D0), -INFINITY);
    int end_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(end), -INFINITY);

    int M0_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(M0), -INFINITY);
    int I0_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(I0), -INFINITY);

    imm_hmm_set_trans(hmm, start_id, D0_id, log(0.1));
    imm_hmm_set_trans(hmm, D0_id, end_id, log(1.0));
    imm_hmm_set_trans(hmm, start_id, M0_id, log(0.5));
    imm_hmm_set_trans(hmm, M0_id, end_id, log(0.8));
    imm_hmm_set_trans(hmm, M0_id, I0_id, log(0.1));
    imm_hmm_set_trans(hmm, I0_id, I0_id, log(0.2));
    imm_hmm_set_trans(hmm, I0_id, end_id, log(1.0));

    TEST_ASSERT_EQUAL_DOUBLE(log(0.1) + log(1.0), imm_hmm_viterbi(hmm, "", end_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.1), imm_hmm_viterbi(hmm, "", D0_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(1.0), imm_hmm_viterbi(hmm, "", start_id));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "", M0_id));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", start_id));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", D0_id));
    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "A", I0_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.4), imm_hmm_viterbi(hmm, "A", M0_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.4) + log(0.8),
                             imm_hmm_viterbi(hmm, "A", end_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.2), imm_hmm_viterbi(hmm, "B", M0_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.2) + log(0.8),
                             imm_hmm_viterbi(hmm, "B", end_id));

    TEST_ASSERT_DOUBLE_IS_NEG_INF(imm_hmm_viterbi(hmm, "AA", M0_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.4) + log(0.1) + log(0.5),
                             imm_hmm_viterbi(hmm, "AA", end_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.4) + log(0.1) + log(0.5),
                             imm_hmm_viterbi(hmm, "AA", end_id));
    TEST_ASSERT_EQUAL_DOUBLE(log(0.5) + log(0.4) + log(0.1) + log(0.2) + 2 * log(0.5),
                             imm_hmm_viterbi(hmm, "AAB", end_id));

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
    struct imm_abc *abc = imm_abc_create("ABCD");
    struct imm_hmm *hmm = imm_hmm_create(abc);

    struct imm_mute_state *start = imm_mute_state_create("START", abc);

    double ins_lprobs[] = {log(0.1), log(0.1), log(0.1), log(0.7)};

    double M0_lprobs[] = {log(0.4), -INFINITY, log(0.6), -INFINITY};
    double M1_lprobs[] = {log(0.6), -INFINITY, log(0.4), -INFINITY};
    double M2_lprobs[] = {0, 0, 0, 0};

    struct imm_normal_state *M0 = imm_normal_state_create("M0", abc, M0_lprobs);
    struct imm_normal_state *I0 = imm_normal_state_create("I0", abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_create("D1", abc);
    struct imm_normal_state *M1 = imm_normal_state_create("M1", abc, M1_lprobs);
    struct imm_normal_state *I1 = imm_normal_state_create("I1", abc, ins_lprobs);

    struct imm_mute_state *D2 = imm_mute_state_create("D2", abc);
    struct imm_normal_state *M2 = imm_normal_state_create("M2", abc, M2_lprobs);

    struct imm_mute_state *end = imm_mute_state_create("END", abc);

    int start_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(start), 0.0);

    int M0_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(M0), -INFINITY);
    int I0_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(I0), -INFINITY);

    int D1_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(D1), -INFINITY);
    int M1_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(M1), -INFINITY);
    int I1_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(I1), -INFINITY);

    int D2_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(D2), -INFINITY);
    int M2_id = imm_hmm_add_state(hmm, imm_normal_state_cast_c(M2), -INFINITY);

    int end_id = imm_hmm_add_state(hmm, imm_mute_state_cast_c(end), -INFINITY);

    imm_hmm_set_trans(hmm, start_id, M0_id, 0.0);
    imm_hmm_set_trans(hmm, start_id, M1_id, 0.0);
    imm_hmm_set_trans(hmm, start_id, M2_id, 0.0);
    imm_hmm_set_trans(hmm, M0_id, M1_id, 0.0);
    imm_hmm_set_trans(hmm, M0_id, M2_id, 0.0);
    imm_hmm_set_trans(hmm, M0_id, end_id, 0.0);
    imm_hmm_set_trans(hmm, M1_id, M2_id, 0.0);
    imm_hmm_set_trans(hmm, M1_id, end_id, 0.0);
    imm_hmm_set_trans(hmm, M2_id, end_id, 0.0);

    imm_hmm_set_trans(hmm, M0_id, I0_id, log(0.2));
    imm_hmm_set_trans(hmm, M0_id, D1_id, log(0.1));
    imm_hmm_set_trans(hmm, I0_id, I0_id, log(0.5));
    imm_hmm_set_trans(hmm, I0_id, M1_id, log(0.5));

    imm_hmm_set_trans(hmm, M1_id, D2_id, log(0.1));
    imm_hmm_set_trans(hmm, M1_id, I1_id, log(0.2));
    imm_hmm_set_trans(hmm, I1_id, I1_id, log(0.5));
    imm_hmm_set_trans(hmm, I1_id, M2_id, log(0.5));
    imm_hmm_set_trans(hmm, D1_id, D2_id, log(0.3));
    imm_hmm_set_trans(hmm, D1_id, M2_id, log(0.7));

    imm_hmm_set_trans(hmm, D2_id, end_id, log(1.0));

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

