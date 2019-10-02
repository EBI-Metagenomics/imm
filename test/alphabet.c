#include "imm.h"
#include "unity.h"

void test_alphabet(void);
void test_alphabet_any_symbol(void);
void test_alphabet_symbol_range(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_alphabet);
    RUN_TEST(test_alphabet_any_symbol);
    RUN_TEST(test_alphabet_symbol_range);
    return UNITY_END();
}

void test_alphabet(void)
{
    struct imm_alphabet *a = imm_alphabet_create("ACGT");
    TEST_ASSERT_NOT_NULL(a);
    imm_alphabet_destroy(a);
}

void test_alphabet_any_symbol(void)
{
    struct imm_alphabet *a = imm_alphabet_create("AC*T");
    TEST_ASSERT_NULL(a);
}

void test_alphabet_symbol_range(void)
{
    char symbols[] = {(char)128, '\0'};
    struct imm_alphabet *a = imm_alphabet_create(symbols);

    TEST_ASSERT_NULL(a);

    imm_alphabet_destroy(a);
}
