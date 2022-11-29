#include "hope.h"
#include "imm/imm.h"

void test_abc_normal(void);
void test_abc_duplicated_alphabet(void);
void test_abc_duplicated_any_symbol(void);
void test_abc_symbol_outside_range(void);
void test_abc_any_symbol_outside_range(void);
void test_abc_union_size(void);

int main(void)
{
    test_abc_normal();
    test_abc_duplicated_alphabet();
    test_abc_duplicated_any_symbol();
    test_abc_symbol_outside_range();
    test_abc_any_symbol_outside_range();
    test_abc_union_size();
    return hope_status();
}

void test_abc_normal(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("ACGT"), '*'), IMM_OK);
    eq(imm_abc_symbol_idx(&abc, 'G'), 2);
    eq(imm_abc_size(&abc), 4);
    cond(imm_abc_has_symbol(&abc, 'C'));
    cond(!imm_abc_has_symbol(&abc, 'L'));
    eq(imm_abc_symbol_type(&abc, 'A'), IMM_SYM_NORMAL);
    eq(imm_abc_symbol_type(&abc, '*'), IMM_SYM_ANY);
    eq(imm_abc_symbol_type(&abc, 'L'), IMM_SYM_NULL);
    eq(imm_abc_typeid(&abc), IMM_ABC);
}

void test_abc_duplicated_alphabet(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_DUPLICATED_SYMBOLS);
}

void test_abc_duplicated_any_symbol(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("AC*T"), '*'), IMM_ANY_SYMBOL_IN_ABC);
}

void test_abc_symbol_outside_range(void)
{
    struct imm_abc abc = imm_abc_empty;
    char symbols[] = {3, '\0'};
    eq(imm_abc_init(&abc, (struct imm_str){2, symbols}, '*'),
       IMM_SYMBOL_OUT_OF_RANGE);
}

void test_abc_any_symbol_outside_range(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("ACGT"), 3), IMM_ANY_SYMBOL_OUT_RANGE);
}

void test_abc_union_size(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("ACT"), '*'), IMM_OK);
    char data[] = "ACAAAAAAAAC*AATT*G";
    eq(imm_abc_union_size(&abc, imm_str(data)), 1);
}
