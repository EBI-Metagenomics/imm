#include "imm.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_silent_state(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hmm_state_id);
    RUN_TEST(test_hmm_del_get_state);
    RUN_TEST(test_hmm_set_trans);
    RUN_TEST(test_hmm_likelihood_single_state);
    RUN_TEST(test_hmm_likelihood_two_states);
    RUN_TEST(test_hmm_likelihood_silent_state);
    return UNITY_END();
}

void test_hmm_state_id(void)
{
    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");
    struct imm_state *state = imm_state_create_silent("State0", alphabet);
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    int state_id = imm_hmm_add_state(hmm, state, log(1.0));
    TEST_ASSERT_EQUAL_INT(0, state_id);

    state_id = imm_hmm_add_state(hmm, state, log(1.0));
    TEST_ASSERT_EQUAL_INT(1, state_id);

    imm_hmm_destroy(hmm);
    imm_state_destroy(state);
    imm_alphabet_destroy(alphabet);
}

void test_hmm_del_get_state(void)
{
    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");
    struct imm_state *state0 = imm_state_create_silent("State0", alphabet);
    struct imm_state *state1 = imm_state_create_silent("State1", alphabet);
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    int state_id0 = imm_hmm_add_state(hmm, state0, log(0.5));
    int state_id1 = imm_hmm_add_state(hmm, state1, log(0.5));
    TEST_ASSERT_EQUAL_INT(IMM_INVALID_STATE_ID,
                          imm_hmm_add_state(hmm, NULL, log(0.5)));

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
    imm_state_destroy(state0);
    imm_state_destroy(state1);
    imm_alphabet_destroy(alphabet);
}

void test_hmm_set_trans(void)
{

    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");
    struct imm_state *state0 = imm_state_create_silent("State0", alphabet);
    struct imm_state *state1 = imm_state_create_silent("State1", alphabet);
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    int state_id0 = imm_hmm_add_state(hmm, state0, log(0.5));
    int state_id1 = imm_hmm_add_state(hmm, state1, log(0.5));

    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id0));
    TEST_ASSERT_NOT_NULL(imm_hmm_get_state(hmm, state_id1));

    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_set_trans(hmm, state_id0, 5, log(0.5)));
    TEST_ASSERT_EQUAL_INT(-1, imm_hmm_set_trans(hmm, 5, state_id1, log(0.5)));
    TEST_ASSERT_EQUAL_INT(0, imm_hmm_set_trans(hmm, state_id0, state_id1, log(0.5)));

    imm_hmm_destroy(hmm);
    imm_state_destroy(state0);
    imm_state_destroy(state1);
    imm_alphabet_destroy(alphabet);
}

void test_hmm_likelihood_single_state(void)
{
    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};

    struct imm_state *state = imm_state_create_normal("State0", alphabet, lprobs);
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    int state_id = imm_hmm_add_state(hmm, state, log(0.5));
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
    imm_state_destroy(state);
    imm_alphabet_destroy(alphabet);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    double lprobs0[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_state *state0 = imm_state_create_normal("State0", alphabet, lprobs0);

    double lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
    struct imm_state *state1 = imm_state_create_normal("State1", alphabet, lprobs1);

    int state_id0 = imm_hmm_add_state(hmm, state0, log(1.0));
    int state_id1 = imm_hmm_add_state(hmm, state1, -INFINITY);

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

    TEST_ASSERT_EQUAL_INT(0, imm_state_normalize(state1));
    imm_path_create(&path);
    imm_path_add(&path, state_id0, 1);
    imm_path_add(&path, state_id1, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0.14814814814815,
                             exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_state_destroy(state0);
    imm_state_destroy(state1);
    imm_alphabet_destroy(alphabet);
}

void test_hmm_likelihood_silent_state(void)
{
    struct imm_alphabet *alphabet = imm_alphabet_create("ACGT");
    struct imm_hmm *hmm = imm_hmm_create(alphabet);

    struct imm_state *state = imm_state_create_silent("State0", alphabet);
    TEST_ASSERT_EQUAL_INT(0, imm_state_normalize(state));

    int state_id = imm_hmm_add_state(hmm, state, log(1.0));

    imm_hmm_set_trans(hmm, state_id, state_id, log(0.1));
    TEST_ASSERT_EQUAL_INT(0, imm_hmm_normalize(hmm));

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

    TEST_ASSERT_EQUAL_INT(0, imm_state_normalize(state));
    imm_path_create(&path);
    imm_path_add(&path, state_id, 1);
    imm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(0, exp(imm_hmm_likelihood(hmm, "GT", path)));
    imm_path_destroy(&path);

    imm_hmm_destroy(hmm);
    imm_state_destroy(state);
    imm_alphabet_destroy(alphabet);
}
