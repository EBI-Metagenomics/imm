#include "cass/cass.h"
#include "imm/imm.h"

void test_path(void);

int main(void)
{
    test_path();
    return cass_status();
}

void test_path(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');

    imm_float                lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_lprob_zero()};
    struct imm_normal_state* state0 = imm_normal_state_create(0, "State0", abc, lprobs);
    struct imm_normal_state* state1 = imm_normal_state_create(1, "State1", abc, lprobs);

    struct imm_path* path = imm_path_create();

    imm_path_append(path, imm_step_create(imm_normal_state_super(state0), 1));
    imm_path_append(path, imm_step_create(imm_normal_state_super(state1), 1));

    cass_cond(imm_step_create(imm_normal_state_super(state1), 2) == NULL);
    cass_cond(imm_step_create(imm_normal_state_super(state1), 0) == NULL);

    struct imm_path* new_path = imm_path_clone(path);

    struct imm_step const* s0 = imm_path_first(path);
    struct imm_step const* s1 = imm_path_first(new_path);
    while (s0) {
        cass_cond(s1 != NULL);
        cass_cond(imm_step_state(s0) == imm_step_state(s1));
        cass_cond(imm_step_seq_len(s0) == imm_step_seq_len(s1));
        s0 = imm_path_next(path, s0);
        s1 = imm_path_next(path, s1);
    }

    imm_path_destroy(path);
    imm_path_destroy(new_path);

    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}
