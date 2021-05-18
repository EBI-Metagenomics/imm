#include "hope/hope.h"
#include "imm/imm.h"
#include "model_frame.h"

void test_large_frame(void);

int main(void)
{
    model_frame_init();
    test_large_frame();
    model_frame_deinit();
    return hope_status();
}

void test_large_frame(void)
{
    struct model_frame *m = &model_frame;
    struct imm_dp *dp = imm_hmm_new_dp(m->hmm, imm_super(m->end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_seq seq = imm_seq(imm_str(model_frame_str), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -1622.8488101101);

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
}
