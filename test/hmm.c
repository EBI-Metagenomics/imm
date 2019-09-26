#include "nhmm.h"
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hmm_state_id);
    RUN_TEST(test_hmm_del_get_state);
    RUN_TEST(test_hmm_set_trans);
    RUN_TEST(test_hmm_likelihood_single_state);
    RUN_TEST(test_hmm_likelihood_two_states);
    return UNITY_END();
}

void test_hmm_state_id(void)
{
    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");
    struct nhmm_state *state = nhmm_state_create_silent("State0", alphabet);
    struct nhmm_hmm *hmm = nhmm_hmm_create(alphabet);

    int state_id = nhmm_hmm_add_state(hmm, state, log(1.0));
    TEST_ASSERT_EQUAL_INT(0, state_id);

    state_id = nhmm_hmm_add_state(hmm, state, log(1.0));
    TEST_ASSERT_EQUAL_INT(1, state_id);

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state);
    nhmm_alphabet_destroy(alphabet);
}


void test_hmm_del_get_state(void)
{
    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");
    struct nhmm_state *state0 = nhmm_state_create_silent("State0", alphabet);
    struct nhmm_state *state1 = nhmm_state_create_silent("State1", alphabet);
    struct nhmm_hmm *hmm = nhmm_hmm_create(alphabet);

    int state_id0 = nhmm_hmm_add_state(hmm, state0, log(0.5));
    int state_id1 = nhmm_hmm_add_state(hmm, state1, log(0.5));

    TEST_ASSERT_EQUAL_INT(0, state_id0);
    TEST_ASSERT_EQUAL_INT(1, state_id1);

    TEST_ASSERT_NOT_NULL(nhmm_hmm_get_state(hmm, state_id0));
    nhmm_hmm_del_state(hmm, state_id0);
    TEST_ASSERT_NULL(nhmm_hmm_get_state(hmm, state_id0));

    TEST_ASSERT_NOT_NULL(nhmm_hmm_get_state(hmm, state_id1));
    nhmm_hmm_del_state(hmm, state_id1);
    TEST_ASSERT_NULL(nhmm_hmm_get_state(hmm, state_id1));

    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_del_state(hmm, state_id0));
    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_del_state(hmm, state_id1));

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state0);
    nhmm_state_destroy(state1);
    nhmm_alphabet_destroy(alphabet);
}


void test_hmm_set_trans(void)
{

    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");
    struct nhmm_state *state0 = nhmm_state_create_silent("State0", alphabet);
    struct nhmm_state *state1 = nhmm_state_create_silent("State1", alphabet);
    struct nhmm_hmm *hmm = nhmm_hmm_create(alphabet);

    int state_id0 = nhmm_hmm_add_state(hmm, state0, log(0.5));
    int state_id1 = nhmm_hmm_add_state(hmm, state1, log(0.5));

    TEST_ASSERT_NOT_NULL(nhmm_hmm_get_state(hmm, state_id0));
    TEST_ASSERT_NOT_NULL(nhmm_hmm_get_state(hmm, state_id1));

    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_set_trans(hmm, state_id0, 5, log(0.5)));
    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_set_trans(hmm, 5, state_id1, log(0.5)));
    TEST_ASSERT_EQUAL_INT(0, nhmm_hmm_set_trans(hmm, state_id0, state_id1, log(0.5)));

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state0);
    nhmm_state_destroy(state1);
    nhmm_alphabet_destroy(alphabet);
}

void test_hmm_likelihood_single_state(void)
{
    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};

    struct nhmm_state *state = nhmm_state_create_normal("State0", alphabet, lprobs);
    struct nhmm_hmm *hmm = nhmm_hmm_create(alphabet);

    int state_id = nhmm_hmm_add_state(hmm, state, log(0.5));
    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_normalize(hmm));

    struct nhmm_path *path = NULL;
    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-1.386294361120, nhmm_hmm_likelihood(hmm, "A", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 2);
    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, "A", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, "AG", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_hmm_likelihood(hmm, "H", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, 5, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, "A", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NEG_INF(nhmm_hmm_likelihood(hmm, "AA", path));
    nhmm_path_destroy(&path);

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, NULL, path));
    nhmm_path_destroy(&path);

    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, "A", NULL));
    TEST_ASSERT_DOUBLE_IS_NAN(nhmm_hmm_likelihood(hmm, NULL, NULL));

    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_normalize(hmm));
    nhmm_hmm_set_trans(hmm, state_id, state_id, -INFINITY);
    TEST_ASSERT_EQUAL_INT(-1, nhmm_hmm_normalize(hmm));
    nhmm_hmm_set_trans(hmm, state_id, state_id, log(0.5));

    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-3.465735902800, nhmm_hmm_likelihood(hmm, "AA", path));
    nhmm_path_destroy(&path);

    TEST_ASSERT_EQUAL_INT(0, nhmm_hmm_normalize(hmm));
    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-2.772588722240, nhmm_hmm_likelihood(hmm, "AA", path));
    nhmm_path_destroy(&path);

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state);
    nhmm_alphabet_destroy(alphabet);
}

void test_hmm_likelihood_two_states(void)
{
    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};

    struct nhmm_state *state = nhmm_state_create_normal("State0", alphabet, lprobs);
    struct nhmm_hmm *hmm = nhmm_hmm_create(alphabet);

    int state_id = nhmm_hmm_add_state(hmm, state, log(1.0));
    nhmm_hmm_set_trans(hmm, state_id, state_id, log(1.0));

    struct nhmm_path *path = NULL;
    nhmm_path_create(&path);
    nhmm_path_add(&path, state_id, 1);
    nhmm_path_add(&path, state_id, 1);
    TEST_ASSERT_EQUAL_DOUBLE(-2.772588722240, nhmm_hmm_likelihood(hmm, "AA", path));
    nhmm_path_destroy(&path);

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state);
    nhmm_alphabet_destroy(alphabet);
}
