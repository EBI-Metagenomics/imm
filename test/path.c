#include "nhmm.h"
#include "unity.h"

void test_path(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_path);
    return UNITY_END();
}

void test_path(void)
{
    struct nhmm_alphabet *alphabet = nhmm_alphabet_create("ACGT");
    struct nhmm_path *path;

    nhmm_path_create(&path);

    struct nhmm_state *state0 = nhmm_state_create_silent("state0", alphabet);
    struct nhmm_state *state1 = nhmm_state_create_silent("state1", alphabet);

    nhmm_path_add(&path, state0, 0);
    nhmm_path_add(&path, state1, 0);

    nhmm_path_destroy(&path);

    nhmm_alphabet_destroy(alphabet);
    nhmm_state_destroy(state0);
    nhmm_state_destroy(state1);
}
