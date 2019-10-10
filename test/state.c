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

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_normal_state *state = imm_normal_state_create("State0", abc, lprobs);
    const struct imm_state *s = imm_normal_state_cast_c(state);

    cass_condition(strcmp(imm_state_get_name(s), "State0") == 0);
    cass_close(log(0.25), imm_state_lprob(s, "A", 1));
    cass_close(log(0.25), imm_state_lprob(s, "C", 1));
    cass_close(log(0.5), imm_state_lprob(s, "G", 1));
    cass_condition(imm_isninf(imm_state_lprob(s, "T", 1)));

    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_mute_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    struct imm_mute_state *state = imm_mute_state_create("State0", abc);
    const struct imm_state *s = imm_mute_state_cast_c(state);

    cass_condition(strcmp(imm_state_get_name(s), "State0") == 0);
    cass_close(0.0, imm_state_lprob(s, "", 0));
    cass_condition(imm_isninf(imm_state_lprob(s, "A", 1)));

    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_table_state(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    struct imm_table_state *state = imm_table_state_create("S0", abc);
    const struct imm_state *s = imm_table_state_cast_c(state);

    cass_condition(strcmp(imm_state_get_name(s), "S0") == 0);
    cass_condition(imm_isninf(imm_state_lprob(s, "", 0)));
    cass_condition(imm_isninf(imm_state_lprob(s, "AGT", 3)));

    imm_table_state_add(state, "GG", log(0.5));
    cass_close(log(0.5), imm_state_lprob(s, "GG", 2));
    cass_condition(imm_isninf(imm_state_lprob(s, "GGT", 3)));

    imm_table_state_add(state, "", log(0.1));
    cass_close(log(0.1), imm_state_lprob(s, "", 0));

    cass_condition(imm_table_state_normalize(state) == 0);

    cass_close(log(0.5 / 0.6), imm_state_lprob(s, "GG", 2));
    cass_close(log(0.1 / 0.6), imm_state_lprob(s, "", 0));

    imm_table_state_destroy(state);
    imm_abc_destroy(abc);
}
