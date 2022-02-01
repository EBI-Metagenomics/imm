#include "cwpack_stream_context.h"
#include "hope/hope.h"
#include "imm/imm.h"

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
    imm_example1_init(IMM_EXAMPLE1_SIZE);
    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp),
       IMM_SUCCESS);

    new_pack_ctx(TMPDIR "/dp_normal.imm");
    EQ(imm_dp_pack(&dp, pack_ctx), IMM_SUCCESS);
    del_pack_ctx();
    imm_del(&dp);

    imm_dp_init(&dp, &m->code);
    new_unpack_ctx(TMPDIR "/dp_normal.imm");
    EQ(imm_dp_unpack(&dp, unpack_ctx), IMM_SUCCESS);
    del_unpack_ctx();

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

void test_dp_io_large_frame(void)
{
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

    new_pack_ctx(TMPDIR "/dp_frame.imm");
    EQ(imm_dp_pack(&dp, pack_ctx), IMM_SUCCESS);
    del_pack_ctx();
    imm_del(&dp);

    imm_dp_init(&dp, &m->code);
    new_unpack_ctx(TMPDIR "/dp_frame.imm");
    EQ(imm_dp_unpack(&dp, unpack_ctx), IMM_SUCCESS);
    del_unpack_ctx();

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
