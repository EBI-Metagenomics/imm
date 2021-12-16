#include "hope/hope.h"
#include "imm/imm.h"

void test_large_normal(void);
void test_large_frame(void);

int main(void)
{
    imm_example1_init(IMM_EXAMPLE1_SIZE);
    test_large_normal();

    imm_example2_init();
    test_large_frame();

    return hope_status();
}

void test_large_normal(void)
{
    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -65826.0106185297);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_frame(void)
{
    struct imm_example2 *m = &imm_example2;
    struct imm_dp dp;
    imm_hmm_init_dp(&m->hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_seq seq = imm_seq(imm_str(imm_example2_seq), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -1622.8488101101);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}
