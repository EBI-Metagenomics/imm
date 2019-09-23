#include "nhmm.h"
#include "unity.h"

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hmm_state_id);
    RUN_TEST(test_hmm_del_get_state);
    RUN_TEST(test_hmm_set_trans);
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

    nhmm_hmm_set_trans(hmm, state_id0, state_id1, log(0.5));

    nhmm_hmm_destroy(hmm);
    nhmm_state_destroy(state0);
    nhmm_state_destroy(state1);
    nhmm_alphabet_destroy(alphabet);
}
