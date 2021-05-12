#include "hope/hope.h"
#include "imm/imm.h"
#include "model_normal.h"

void test_large_normal(void);

int main(void)
{
    model_normal_init();
    test_large_normal();
    model_normal_deinit();
    return hope_status();
}

void test_large_normal(void)
{
    struct model_normal *m = &model_normal;
    struct imm_dp *dp = imm_hmm_new_dp(model_normal.hmm, imm_super(m->end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    struct imm_seq seq = imm_seq(imm_str(model_normal_str), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -65826.0106185297);

    imm_del(task);
    imm_deinit(&result);
    imm_del(dp);
}
