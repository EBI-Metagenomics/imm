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
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    double lprobs[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
    struct imm_normal_state const* state = imm_normal_state_create("State0", abc, lprobs);

    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "State0") == 0);
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("A")), log(0.25));
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("C")), log(0.25));
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("G")), log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("T"))));
#ifdef DEBUG
    cass_cond(!imm_lprob_is_valid(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("X"))));
#endif

    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    struct imm_mute_state const* state = imm_mute_state_create("State0", abc);

    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "State0") == 0);
    cass_close(0.0, imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("")));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("A"))));

    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_table_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    struct imm_seq_table*   table = imm_seq_table_create(abc);
    struct imm_table_state* state = imm_table_state_create("S0", table);
    cass_cond(strcmp(imm_state_get_name(imm_state_cast_c(state)), "S0") == 0);
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ(""))));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("AGT"))));
    imm_table_state_destroy(state);

    table = imm_seq_table_create(abc);
    cass_cond(imm_seq_table_add(table, IMM_SEQ("GG"), log(0.5)) == 0);
    cass_cond(imm_seq_table_add(table, IMM_SEQ(""), log(0.1)) == 0);
    cass_cond(imm_seq_table_add(table, IMM_SEQ("H"), log(0.1)) == 1);
    state = imm_table_state_create("S0", table);
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("GG")), log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("GGT"))));
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("")), log(0.1));
    imm_table_state_destroy(state);

    table = imm_seq_table_create(abc);
    cass_cond(imm_seq_table_add(table, IMM_SEQ("GG"), log(0.5)) == 0);
    cass_cond(imm_seq_table_add(table, IMM_SEQ(""), log(0.1)) == 0);
    cass_cond(imm_seq_table_add(table, IMM_SEQ("H"), log(0.1)) == 1);
    cass_cond(imm_seq_table_normalize(table) == 0);
    state = imm_table_state_create("S0", table);
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("GG")), log(0.5 / 0.6));
    cass_close(imm_state_lprob(imm_state_cast_c(state), IMM_SEQ("")), log(0.1 / 0.6));
    imm_table_state_destroy(state);

    imm_abc_destroy(abc);
}
