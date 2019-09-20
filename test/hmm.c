#include "nhmm.h"
#include "unity.h"

void test_hmm_state_id(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_hmm_state_id);
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

