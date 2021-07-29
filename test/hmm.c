#include "hope/hope.h"
#include "imm/imm.h"

void test_hmm_state_id(void);
void test_hmm_set_trans(void);
void test_hmm_wrong_states(void);

struct imm_abc abc;

int main(void)
{
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');

    test_hmm_state_id();
    test_hmm_set_trans();
    test_hmm_wrong_states();
    return hope_status();
}

void test_hmm_state_id(void)
{
    struct imm_mute_state state;
    imm_mute_state_init(&state, 0, &abc);
    struct imm_hmm hmm = imm_hmm_init(&abc);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_SUCCESS);
    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_ILLEGALARG);
}

void test_hmm_set_trans(void)
{
    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);
    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 1, &abc);
    struct imm_hmm hmm = imm_hmm_init(&abc);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state0)), IMM_SUCCESS);
    EQ(imm_hmm_add_state(&hmm, imm_super(&state1)), IMM_SUCCESS);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                         imm_log(0.5)),
       IMM_SUCCESS);
}

void test_hmm_wrong_states(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);

    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);
    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 0, &abc);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state0)), IMM_SUCCESS);

    EQ(imm_hmm_set_start(&hmm, imm_super(&state1), imm_log(0.3)),
       IMM_ILLEGALARG);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                         imm_log(0.3)),
       IMM_ILLEGALARG);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state0),
                         imm_log(0.3)),
       IMM_ILLEGALARG);

    EQ(imm_hmm_normalize_state_trans(&hmm, imm_mute_state_super(&state1)),
       IMM_ILLEGALARG);

    EQ(imm_hmm_normalize_state_trans(&hmm, imm_mute_state_super(&state0)),
       IMM_SUCCESS);
}
