#include "imm.h"
#include "cass/cass.h"

void test_path(void);

int main(void)
{
    test_path();
    return cass_status();
}

void test_path(void)
{
    struct imm_abc *abc = imm_abc_create("ACGT");

    double lprobs[] = {log(0.25), log(0.25), log(0.5), -INFINITY};
    struct imm_state *state0 = imm_normal_state_create("State0", abc, lprobs);
    struct imm_state *state1 = imm_normal_state_create("State1", abc, lprobs);

    struct imm_path *path = NULL;
    imm_path_create(&path);

    imm_path_add(&path, state0, 1);
    imm_path_add(&path, state1, 1);

    imm_path_destroy(&path);

    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
}
