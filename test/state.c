#include "cass/cass.h"
#include "imm.h"

void test_normal_state(void);
void test_mute_state(void);
void test_table_state(void);

int main(void)
{
    test_normal_state();
    test_mute_state();
    test_table_state();
    return cass_status();
}

void test_normal_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), LOG0};
    struct imm_state *state = imm_normal_state_create("State0", abc, lprobs);

    cass_condition(strcmp(imm_state_get_name(state), "State0") == 0);
    cass_close(log(0.25), imm_state_lprob(state, "A", 1));
    cass_close(log(0.25), imm_state_lprob(state, "C", 1));
    cass_close(log(0.5), imm_state_lprob(state, "G", 1));
    cass_condition(imm_isninf(imm_state_lprob(state, "T", 1)));

    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_mute_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    struct imm_state *state = imm_mute_state_create("State0", abc);

    cass_condition(strcmp(imm_state_get_name(state), "State0") == 0);
    cass_close(0.0, imm_state_lprob(state, "", 0));
    cass_condition(imm_isninf(imm_state_lprob(state, "A", 1)));

    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_table_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    struct imm_state *state = imm_table_state_create("S0", abc);

    cass_condition(strcmp(imm_state_get_name(state), "S0") == 0);
    cass_condition(imm_isninf(imm_state_lprob(state, "", 0)));
    cass_condition(imm_isninf(imm_state_lprob(state, "AGT", 3)));

    imm_table_state_add(state, "GG", log(0.5));
    cass_close(log(0.5), imm_state_lprob(state, "GG", 2));
    cass_condition(imm_isninf(imm_state_lprob(state, "GGT", 3)));

    imm_table_state_add(state, "", log(0.1));
    cass_close(log(0.1), imm_state_lprob(state, "", 0));

    cass_condition(imm_table_state_normalize(state) == 0);

    cass_close(log(0.5 / 0.6), imm_state_lprob(state, "GG", 2));
    cass_close(log(0.1 / 0.6), imm_state_lprob(state, "", 0));

    imm_table_state_destroy(state);
    imm_abc_destroy(abc);
}
