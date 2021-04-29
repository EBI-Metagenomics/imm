#include "cass/cass.h"
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
    return cass_status();
}

void test_abc_normal(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    cass_cond(abc != NULL);
    cass_cond(imm_abc_symbol(abc, 2) == 'G');
    cass_cond(imm_abc_symbol_idx(abc, 'G') == 2);
    cass_cond(imm_abc_len(abc) == 4);
    cass_cond(imm_abc_has_symbol(abc, 'C'));
    cass_cond(!imm_abc_has_symbol(abc, 'L'));
    cass_cond(imm_abc_symbol_type(abc, 'A') == IMM_SYM_NORMAL);
    cass_cond(imm_abc_symbol_type(abc, '*') == IMM_SYM_ANY);
    cass_cond(imm_abc_symbol_type(abc, 'L') == IMM_SYM_NULL);
    imm_abc_del(abc);
}

void test_abc_duplicated_alphabet(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACTC", '*');
    cass_cond(abc == NULL);
}

void test_abc_duplicated_any_symbol(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "AC*T", '*');
    cass_cond(abc == NULL);
}

void test_abc_symbol_outside_range(void)
{
    char symbols[] = {3, '\0'};
    struct imm_abc const *abc = imm_abc_new(4, symbols, '*');
    cass_cond(abc == NULL);
}

void test_abc_any_symbol_outside_range(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", 3);
    cass_cond(abc == NULL);
}
