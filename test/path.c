#include "cass/cass.h"
#include "imm.h"

void test_path(void);

int main(void)
{
    test_path();
    return cass_status();
}

void test_path(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
    struct imm_normal_state* state0 = imm_normal_state_create("State0", abc, lprobs);
    struct imm_normal_state* state1 = imm_normal_state_create("State1", abc, lprobs);

    struct imm_path* path = imm_path_create();

    cass_cond(imm_path_append(path, imm_state_cast_c(state0), 1) == 0);
    cass_cond(imm_path_append(path, imm_state_cast_c(state1), 1) == 0);

    cass_cond(imm_path_append(path, imm_state_cast_c(state1), 2) == 1);
    cass_cond(imm_path_append(path, imm_state_cast_c(state1), 0) == 1);
    cass_cond(imm_path_append(path, imm_state_cast_c(state1), -1) == 1);

    imm_path_destroy(path);

    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}
