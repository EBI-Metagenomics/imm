#include "hope/hope.h"
#include "imm/imm.h"

void test_abc_normal(void);
void test_abc_duplicated_alphabet(void);
void test_abc_duplicated_any_symbol(void);
void test_abc_symbol_outside_range(void);
void test_abc_any_symbol_outside_range(void);

int main(void)
{
    test_abc_normal();
    test_abc_duplicated_alphabet();
    test_abc_duplicated_any_symbol();
    test_abc_symbol_outside_range();
    test_abc_any_symbol_outside_range();
    return hope_status();
}

void test_abc_normal(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    NOTNULL(abc);
    EQ(imm_abc_symbol(abc, 2), 'G');
    EQ(imm_abc_symbol_idx(abc, 'G'), 2);
    EQ(imm_abc_len(abc), 4);
    COND(imm_abc_has_symbol(abc, 'C'));
    COND(!imm_abc_has_symbol(abc, 'L'));
    EQ(imm_abc_symbol_type(abc, 'A'), IMM_SYM_NORMAL);
    EQ(imm_abc_symbol_type(abc, '*'), IMM_SYM_ANY);
    EQ(imm_abc_symbol_type(abc, 'L'), IMM_SYM_NULL);
    imm_abc_del(abc);
}

void test_abc_duplicated_alphabet(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACTC", '*');
    ISNULL(abc);
}

void test_abc_duplicated_any_symbol(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "AC*T", '*');
    ISNULL(abc);
}

void test_abc_symbol_outside_range(void)
{
    char symbols[] = {3, '\0'};
    struct imm_abc const *abc = imm_abc_new(4, symbols, '*');
    ISNULL(abc);
}

void test_abc_any_symbol_outside_range(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", 3);
    ISNULL(abc);
}
