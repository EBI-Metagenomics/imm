#include "cass/cass.h"
#include "imm/imm.h"

void test_normal_state(void);
void test_mute_state(void);
void test_table_state(void);
void test_state_destroy(void);

int main(void)
{
    test_normal_state();
    test_mute_state();
    test_table_state();
    test_state_destroy();
    return cass_status();
}

void test_normal_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);

    imm_float                lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero()};
    struct imm_normal_state* state = imm_normal_state_create(0, "State0", abc, lprobs);

    cass_cond(strcmp(imm_state_name(imm_normal_state_super(state)), "State0") == 0);
    cass_close(imm_state_lprob(imm_normal_state_super(state), A), imm_log(0.25));
    cass_close(imm_state_lprob(imm_normal_state_super(state), C), imm_log(0.25));
    cass_close(imm_state_lprob(imm_normal_state_super(state), G), imm_log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_normal_state_super(state), T)));

    struct imm_state* super = imm_normal_state_super(state);
    cass_cond(imm_mute_state_derived(super) == NULL);
    cass_cond((state = imm_normal_state_derived(super)) != NULL);

    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(C);
    imm_seq_destroy(G);
    imm_seq_destroy(T);
}

void test_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);

    struct imm_mute_state* state = imm_mute_state_create(0, "State0", abc);

    cass_cond(strcmp(imm_state_name(imm_mute_state_super(state)), "State0") == 0);
    cass_close(0.0, imm_state_lprob(imm_mute_state_super(state), EMPTY));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_mute_state_super(state), A)));

    struct imm_state* super = imm_mute_state_super(state);
    cass_cond(imm_normal_state_derived(super) == NULL);
    cass_cond((state = imm_mute_state_derived(super)) != NULL);

    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(EMPTY);
}

void test_table_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* AGT = imm_seq_create("AGT", abc);
    struct imm_seq const* GG = imm_seq_create("GG", abc);
    struct imm_seq const* GGT = imm_seq_create("GGT", abc);

    struct imm_seq_table*   table = imm_seq_table_create(abc);
    struct imm_table_state* state = imm_table_state_create(0, "S0", table);
    imm_seq_table_destroy(table);

    cass_cond(strcmp(imm_state_name(imm_table_state_super(state)), "S0") == 0);
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_table_state_super(state), EMPTY)));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_table_state_super(state), AGT)));
    imm_table_state_destroy(state);

    table = imm_seq_table_create(abc);
    cass_cond(imm_seq_table_add(table, GG, (imm_float)imm_log(0.5)) == 0);
    cass_cond(imm_seq_table_add(table, EMPTY, (imm_float)imm_log(0.1)) == 0);
    state = imm_table_state_create(1, "S0", table);
    imm_seq_table_destroy(table);

    cass_close(imm_state_lprob(imm_table_state_super(state), GG), imm_log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_table_state_super(state), GGT)));
    cass_close(imm_state_lprob(imm_table_state_super(state), EMPTY), imm_log(0.1));
    imm_table_state_destroy(state);

    table = imm_seq_table_create(abc);
    cass_cond(imm_seq_table_add(table, GG, (imm_float)imm_log(0.5)) == 0);
    cass_cond(imm_seq_table_add(table, EMPTY, (imm_float)imm_log(0.1)) == 0);
    cass_cond(imm_seq_table_normalize(table) == 0);
    state = imm_table_state_create(2, "S0", table);
    imm_seq_table_destroy(table);

    cass_close(imm_state_lprob(imm_table_state_super(state), GG), imm_log(0.5 / 0.6));
    cass_close(imm_state_lprob(imm_table_state_super(state), EMPTY), imm_log(0.1 / 0.6));

    struct imm_state* super = imm_table_state_super(state);
    cass_cond(imm_normal_state_derived(super) == NULL);
    cass_cond((state = imm_table_state_derived(super)) != NULL);

    imm_table_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(AGT);
    imm_seq_destroy(GG);
    imm_seq_destroy(GGT);
}

void test_state_destroy(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);

    imm_float                lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero()};
    struct imm_normal_state* state = imm_normal_state_create(0, "State0", abc, lprobs);

    cass_cond(strcmp(imm_state_name(imm_normal_state_super(state)), "State0") == 0);
    cass_close(imm_state_lprob(imm_normal_state_super(state), A), imm_log(0.25));
    cass_close(imm_state_lprob(imm_normal_state_super(state), C), imm_log(0.25));
    cass_close(imm_state_lprob(imm_normal_state_super(state), G), imm_log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_normal_state_super(state), T)));

    imm_state_destroy(imm_normal_state_super(state));
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(C);
    imm_seq_destroy(G);
    imm_seq_destroy(T);
}
