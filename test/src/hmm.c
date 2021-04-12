#include "cass/cass.h"
#include "imm/imm.h"

void test_hmm_state_id(void);
void test_hmm_del_get_state(void);
void test_hmm_set_trans(void);
void test_hmm_wrong_states(void);

int main(void)
{
    test_hmm_state_id();
    test_hmm_del_get_state();
    test_hmm_set_trans();
    test_hmm_wrong_states();
    return cass_status();
}

void test_hmm_state_id(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state = imm_mute_state_create("State0", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, imm_mute_state_super(state), imm_log(1.0)) == 0);
    cass_cond(imm_hmm_add_state(hmm, imm_mute_state_super(state), imm_log(1.0)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
}

void test_hmm_del_get_state(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    cass_cond(imm_hmm_add_state(hmm, imm_mute_state_super(state0), imm_log(0.5)) == 0);
    cass_cond(imm_hmm_add_state(hmm, imm_mute_state_super(state1), imm_log(0.5)) == 0);

    cass_cond(imm_hmm_del_state(hmm, imm_mute_state_super(state0)) == 0);
    cass_cond(imm_hmm_del_state(hmm, imm_mute_state_super(state1)) == 0);

    cass_cond(imm_hmm_del_state(hmm, imm_mute_state_super(state0)) == 1);
    cass_cond(imm_hmm_del_state(hmm, imm_mute_state_super(state1)) == 1);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_set_trans(void)
{
    struct imm_abc const*        abc = imm_abc_create("ACGT", '*');
    struct imm_mute_state const* state0 = imm_mute_state_create("State0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("State1", abc);
    struct imm_hmm*              hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0), imm_log(0.5));
    imm_hmm_add_state(hmm, imm_mute_state_super(state1), imm_log(0.5));

    cass_cond(imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_mute_state_super(state1), imm_log(0.5)) == 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}

void test_hmm_wrong_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create("state0", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0), imm_log(0.5));
    cass_equal(imm_hmm_set_start(hmm, imm_mute_state_super(state1), imm_log(0.3)), 1);
    cass_equal(imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_mute_state_super(state1), imm_log(0.3)), 1);
    cass_equal(imm_hmm_set_trans(hmm, imm_mute_state_super(state1), imm_mute_state_super(state0), imm_log(0.3)), 1);
    cass_cond(imm_hmm_create_dp(hmm, imm_mute_state_super(state1)) == NULL);
    cass_equal(imm_hmm_normalize_trans(hmm, imm_mute_state_super(state1)), 1);
    cass_equal(imm_hmm_normalize_trans(hmm, imm_mute_state_super(state0)), 0);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
}
