#include "nhmm.h"
#include "unity.h"

void test_alphabet(void);
void test_alphabet_symbol_range(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_alphabet);
    RUN_TEST(test_alphabet_symbol_range);
    return UNITY_END();
}

void test_alphabet(void)
{
    struct nhmm_alphabet *a = nhmm_alphabet_create("ACGT");

    TEST_ASSERT_EQUAL_INT(4, nhmm_alphabet_length(a));

    nhmm_alphabet_destroy(a);
}


void test_alphabet_symbol_range(void)
{
    char symbols[] = {(char) 128, '\0'};
    struct nhmm_alphabet *a = nhmm_alphabet_create(symbols);

    TEST_ASSERT_NULL(a);

    nhmm_alphabet_destroy(a);
}
