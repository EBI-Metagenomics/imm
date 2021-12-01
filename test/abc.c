#include "hope/hope.h"
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
    EQ(imm_abc_init(&abc, IMM_STR("ACGT"), '*'), IMM_SUCCESS);
    EQ(imm_abc_symbol_idx(&abc, 'G'), 2);
    EQ(imm_abc_size(&abc), 4);
    COND(imm_abc_has_symbol(&abc, 'C'));
    COND(!imm_abc_has_symbol(&abc, 'L'));
    EQ(imm_abc_symbol_type(&abc, 'A'), IMM_SYM_NORMAL);
    EQ(imm_abc_symbol_type(&abc, '*'), IMM_SYM_ANY);
    EQ(imm_abc_symbol_type(&abc, 'L'), IMM_SYM_NULL);
    EQ(imm_abc_typeid(&abc), IMM_ABC);
}

void test_abc_duplicated_alphabet(void)
{
    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_ILLEGALARG);
}

void test_abc_duplicated_any_symbol(void)
{
    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("AC*T"), '*'), IMM_ILLEGALARG);
}

void test_abc_symbol_outside_range(void)
{
    struct imm_abc abc = imm_abc_empty;
    char symbols[] = {3, '\0'};
    EQ(imm_abc_init(&abc, (struct imm_str){2, symbols}, '*'), IMM_ILLEGALARG);
}

void test_abc_any_symbol_outside_range(void)
{
    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACGT"), 3), IMM_ILLEGALARG);
}

void test_abc_union_size(void)
{
    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACT"), '*'), IMM_SUCCESS);
    char data[] = "ACAAAAAAAAC*AATT*G";
    EQ(imm_abc_union_size(&abc, imm_str(data)), 1);
}
