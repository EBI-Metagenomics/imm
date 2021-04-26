#include "cass/cass.h"
#include "imm/imm.h"

void test_normal_state(void);
void test_mute_state(void);

int main(void)
{
    test_normal_state();
    test_mute_state();
    return cass_status();
}

void test_normal_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *C = imm_seq_new(1, "C", abc);
    struct imm_seq const *G = imm_seq_new(1, "G", abc);
    struct imm_seq const *T = imm_seq_new(1, "T", abc);

    imm_float lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                          imm_lprob_zero()};
    struct imm_normal_state *state = imm_normal_state_new(0, abc, lprobs);

    cass_equal(imm_state_id(imm_super(state)), 0);
    cass_close(imm_state_lprob(imm_super(state), A), imm_log(0.25));
    cass_close(imm_state_lprob(imm_super(state), C), imm_log(0.25));
    cass_close(imm_state_lprob(imm_super(state), G), imm_log(0.5));
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_super(state), T)));

    struct imm_state *super = imm_super(state);
    cass_null(imm_mute_state(super));
    state = imm_normal_state(super);
    cass_not_null(state);

    imm_del(state);
    imm_del(abc);
    imm_del(A);
    imm_del(C);
    imm_del(G);
    imm_del(T);
}

void test_mute_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);

    struct imm_mute_state *state = imm_mute_state_new(43, abc);

    cass_equal(imm_state_id(imm_super(state)), 43);
    cass_close(imm_state_lprob(imm_super(state), EMPTY), 0.0);
    cass_cond(imm_lprob_is_zero(imm_state_lprob(imm_super(state), A)));

    struct imm_state *super = imm_super(state);
    cass_null(imm_normal_state(super));
    state = imm_mute_state(super);
    cass_not_null(state);

    imm_del(state);
    imm_del(abc);
    imm_del(A);
    imm_del(EMPTY);
}
