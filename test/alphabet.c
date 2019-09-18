#include "nhmm.h"
#include "unity.h"

void test_alphabet_1(void) {

    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");
    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(a));
    nhmm_alphabet_destroy(a);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_alphabet_1);
    return UNITY_END();
}
