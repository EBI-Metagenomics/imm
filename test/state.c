#include "hope/hope.h"
#include "imm/imm.h"

void test_normal_state(void);
void test_mute_state(void);

int main(void)
{
    test_normal_state();
    test_mute_state();
    return hope_status();
}

void test_normal_state(void)
{
    struct imm_abc abc = imm_abc_empty;
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    struct imm_seq A = imm_seq_init(IMM_STR("A"), &abc);
    struct imm_seq C = imm_seq_init(IMM_STR("C"), &abc);
    struct imm_seq G = imm_seq_init(IMM_STR("G"), &abc);
    struct imm_seq T = imm_seq_init(IMM_STR("T"), &abc);

    imm_float lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                          imm_lprob_zero()};
    struct imm_normal_state *state = imm_normal_state_new(0, &abc, lprobs);

    EQ(imm_state_id(imm_super(state)), 0);
    CLOSE(imm_state_lprob(imm_super(state), &A), imm_log(0.25));
    CLOSE(imm_state_lprob(imm_super(state), &C), imm_log(0.25));
    CLOSE(imm_state_lprob(imm_super(state), &G), imm_log(0.5));
    COND(imm_lprob_is_zero(imm_state_lprob(imm_super(state), &T)));

    imm_del(state);
}

void test_mute_state(void)
{
    struct imm_abc abc = imm_abc_empty;
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    struct imm_seq EMPTY = imm_seq_init(IMM_STR(""), &abc);
    struct imm_seq A = imm_seq_init(IMM_STR("A"), &abc);

    struct imm_mute_state *state = imm_mute_state_new(43, &abc);

    EQ(imm_state_id(imm_super(state)), 43);
    CLOSE(imm_state_lprob(imm_super(state), &EMPTY), 0.0);
    COND(imm_lprob_is_zero(imm_state_lprob(imm_super(state), &A)));

    imm_del(state);
}
