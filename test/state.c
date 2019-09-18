#include "nhmm.h"
#include "unity.h"

void test_state_normal(void) {

    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");
    struct nhmm_state *s = nhmm_state_create_normal("State0", a);

    TEST_ASSERT_EQUAL_STRING("State0", nhmm_state_name(s));

    nhmm_state_destroy(s);
    nhmm_alphabet_destroy(a);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_state_normal);
    return UNITY_END();
}
