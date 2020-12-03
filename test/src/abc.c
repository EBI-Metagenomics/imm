#include "cass.h"
#include "imm/imm.h"

void test_abc_normal(void);
void test_abc_clone(void);
void test_abc_duplicated_alphabet(void);
void test_abc_duplicated_any_symbol(void);
void test_abc_symbol_outside_range(void);
void test_abc_any_symbol_outside_range(void);

int main(void)
{
    test_abc_normal();
    test_abc_clone();
    test_abc_duplicated_alphabet();
    test_abc_duplicated_any_symbol();
    test_abc_symbol_outside_range();
    test_abc_any_symbol_outside_range();
    return cass_status();
}

void test_abc_normal(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    cass_cond(abc != NULL);
    cass_cond(imm_abc_symbol_id(abc, 2) == 'G');
    cass_cond(imm_abc_symbol_idx(abc, 'G') == 2);
    cass_cond(imm_abc_length(abc) == 4);
    cass_cond(imm_abc_has_symbol(abc, 'C'));
    cass_cond(!imm_abc_has_symbol(abc, 'L'));
    cass_cond(imm_abc_symbol_type(abc, 'A') == IMM_SYMBOL_NORMAL);
    cass_cond(imm_abc_symbol_type(abc, '*') == IMM_SYMBOL_ANY);
    cass_cond(imm_abc_symbol_type(abc, 'L') == IMM_SYMBOL_UNKNOWN);
    imm_abc_destroy(abc);
}

void test_abc_clone(void)
{
    struct imm_abc const* a = imm_abc_create("ACGT", '*');
    cass_cond(a != NULL);

    struct imm_abc const* b = imm_abc_clone(a);
    cass_cond(b != NULL);

    cass_cond(imm_abc_length(a) == imm_abc_length(b));
    cass_cond(imm_abc_symbol_idx(a, 'A') == imm_abc_symbol_idx(b, 'A'));
    cass_cond(imm_abc_symbol_id(a, 2) == imm_abc_symbol_id(b, 2));
    cass_cond(imm_abc_any_symbol(a) == imm_abc_any_symbol(b));

    imm_abc_destroy(a);
    imm_abc_destroy(b);
}

void test_abc_duplicated_alphabet(void)
{
    struct imm_abc const* abc = imm_abc_create("ACTC", '*');
    cass_cond(abc == NULL);
}

void test_abc_duplicated_any_symbol(void)
{
    struct imm_abc const* abc = imm_abc_create("AC*T", '*');
    cass_cond(abc == NULL);
}

void test_abc_symbol_outside_range(void)
{
    char                  symbols[] = {3, '\0'};
    struct imm_abc const* abc = imm_abc_create(symbols, '*');
    cass_cond(abc == NULL);
}

void test_abc_any_symbol_outside_range(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", 3);
    cass_cond(abc == NULL);
}
