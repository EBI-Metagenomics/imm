#include "imm.h"
#include "unity.h"

void test_abc(void);
void test_abc_any_symbol(void);
void test_abc_symbol_range(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_abc);
    RUN_TEST(test_abc_any_symbol);
    RUN_TEST(test_abc_symbol_range);
    return UNITY_END();
}

void test_abc(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");
    TEST_ASSERT_NOT_NULL(abc);
    imm_abc_destroy(abc);
}

void test_abc_any_symbol(void)
{
    struct imm_abc *abc = imm_abc_create("AC*T");
    TEST_ASSERT_NULL(abc);
}

void test_abc_symbol_range(void)
{
    char symbols[] = {(char)128, '\0'};
    struct imm_abc *abc = imm_abc_create(symbols);

    TEST_ASSERT_NULL(abc);

    imm_abc_destroy(abc);
}
