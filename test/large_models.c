#include "hope/hope.h"
#include "imm/imm.h"

void test_large_normal(void);
void test_large_frame(void);

int main(void)
{
    imm_example1_init();
    test_large_normal();

    imm_example2_init();
    test_large_frame();

    return hope_status();
}

void test_large_normal(void)
{
    struct imm_example1 *m = &imm_example1;
    struct imm_dp *dp = imm_hmm_new_dp(&imm_example1.hmm, imm_super(&m->end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -65826.0106185297);

    imm_del(task);
    imm_del(&result);
    imm_del(dp);
}

void test_large_frame(void)
{
    struct imm_example2 *m = &imm_example2;
    struct imm_dp *dp = imm_hmm_new_dp(&m->hmm, imm_super(&m->end));
    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();

    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_seq seq = imm_seq(imm_str(imm_example2_seq), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -1622.8488101101);

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
}
