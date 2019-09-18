#include "nhmm.h"
#include "unity.h"

void test_alphabet(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(a));
    TEST_ASSERT_EQUAL_INT(0, nhmm_alphabet_symbol_idx(a, 'A'));
    TEST_ASSERT_EQUAL_INT(1, nhmm_alphabet_symbol_idx(a, 'C'));
    TEST_ASSERT_EQUAL_INT(2, nhmm_alphabet_symbol_idx(a, 'G'));
    TEST_ASSERT_EQUAL_INT(3, nhmm_alphabet_symbol_idx(a, 'T'));

    TEST_ASSERT_EQUAL_INT('A', nhmm_alphabet_symbol_id(a, 0));
    TEST_ASSERT_EQUAL_INT('C', nhmm_alphabet_symbol_id(a, 1));
    TEST_ASSERT_EQUAL_INT('G', nhmm_alphabet_symbol_id(a, 2));
    TEST_ASSERT_EQUAL_INT('T', nhmm_alphabet_symbol_id(a, 3));

    nhmm_alphabet_destroy(a);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_alphabet);
    return UNITY_END();
}
