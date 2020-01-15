#include "cass.h"
#include "imm/imm.h"

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
    struct imm_abc* abc = imm_abc_create("ACGT", '*');

    double                   lprobs[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
    struct imm_normal_state* state = imm_normal_state_create("State0", abc, lprobs);

    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "State0") == 0);
    cass_close(log(0.25), imm_state_lprob(imm_state_cast_c(state), "A", 1));
    cass_close(log(0.25), imm_state_lprob(imm_state_cast_c(state), "C", 1));
    cass_close(log(0.5), imm_state_lprob(imm_state_cast_c(state), "G", 1));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), "T", 1)));
#ifdef DEBUG
    cass_cond(!imm_lprob_is_valid(imm_state_lprob(imm_state_cast_c(state), "X", 1)));
#endif

    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_mute_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT", '*');

    struct imm_mute_state* state = imm_mute_state_create("State0", abc);

    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "State0") == 0);
    cass_close(0.0, imm_state_lprob(imm_state_cast_c(state), "", 0));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), "A", 1)));

    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_table_state(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT", '*');

    struct imm_table_state* state = imm_table_state_create("S0", abc);

    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "S0") == 0);
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), "", 0)));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), "AGT", 3)));

    imm_table_state_add(state, "GG", log(0.5));
    cass_close(log(0.5), imm_state_lprob(imm_state_cast_c(state), "GG", 2));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), "GGT", 3)));

    imm_table_state_add(state, "", log(0.1));
    cass_close(log(0.1), imm_state_lprob(imm_state_cast_c(state), "", 0));

    cass_cond(imm_table_state_normalize(state) == 0);

    cass_close(log(0.5 / 0.6), imm_state_lprob(imm_state_cast_c(state), "GG", 2));
    cass_close(log(0.1 / 0.6), imm_state_lprob(imm_state_cast_c(state), "", 0));

    imm_table_state_destroy(state);
    imm_abc_destroy(abc);
}
