#include "cass.h"
#include "imm/imm.h"

void test_abc(void);
void test_abc_any_symbol(void);
void test_abc_symbol_range(void);

int main(void)
{
    test_abc();
    test_abc_any_symbol();
    test_abc_symbol_range();
    return cass_status();
}

void test_abc(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    cass_cond(abc != NULL);
    imm_abc_destroy(abc);
}

void test_abc_any_symbol(void)
{
    struct imm_abc const* abc = imm_abc_create("AC*T", '*');
    cass_cond(abc == NULL);
}

void test_abc_symbol_range(void)
{
    char                  symbols[] = {(char)128, '\0'};
    struct imm_abc const* abc = imm_abc_create(symbols, '*');

    cass_cond(abc == NULL);

    imm_abc_destroy(abc);
}
