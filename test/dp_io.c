#include "hope.h"
#include "imm/imm.h"
#include "lite_pack/file/file.h"

void test_dp_io_large_normal(void);
void test_dp_io_large_frame(void);

int main(void)
{
    test_dp_io_large_normal();
    test_dp_io_large_frame();
    return hope_status();
}

void test_dp_io_large_normal(void)
{
    struct lip_file file = {0};
    imm_example1_init(IMM_EXAMPLE1_SIZE);
    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp),
       IMM_SUCCESS);

    file.fp = fopen(TMPDIR "/dp_normal.imm", "wb");
    EQ(imm_dp_pack(&dp, &file), IMM_SUCCESS);
    fclose(file.fp);
    imm_del(&dp);

    imm_dp_init(&dp, &m->code);
    file.fp = fopen(TMPDIR "/dp_normal.imm", "rb");
    EQ(imm_dp_unpack(&dp, &file), IMM_SUCCESS);
    fclose(file.fp);

    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();
    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -194581.04361377980);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_dp_io_large_frame(void)
{
    struct lip_file file = {0};
    imm_example2_init();
    struct imm_example2 *m = &imm_example2;
    struct imm_dp dp;
    imm_hmm_init_dp(&m->hmm, imm_super(&m->end), &dp);

    struct imm_abc const *abc = imm_super(imm_super(m->dna));
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();
    struct imm_seq seq = imm_seq(imm_str(imm_example2_seq), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -1622.8488101101);
    imm_del(&prod);
    imm_del(task);

    file.fp = fopen(TMPDIR "/dp_frame.imm", "wb");
    EQ(imm_dp_pack(&dp, &file), IMM_SUCCESS);
    fclose(file.fp);
    imm_del(&dp);

    imm_dp_init(&dp, &m->code);
    file.fp = fopen(TMPDIR "/dp_frame.imm", "rb");
    EQ(imm_dp_unpack(&dp, &file), IMM_SUCCESS);
    fclose(file.fp);

    task = imm_task_new(&dp);
    prod = imm_prod();
    seq = imm_seq(imm_str(imm_example2_seq), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    CLOSE(prod.loglik, -1622.8488101101);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}
