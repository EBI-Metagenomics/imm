#include "hope/hope.h"
#include "imm/imm.h"
#include "model_frame.h"
#include "model_normal.h"

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
    model_normal_init();
    struct model_normal *m = &model_normal;
    struct imm_dp *dp = imm_hmm_new_dp(model_normal.hmm, imm_super(m->end));

    FILE *file = fopen(TMPDIR "/dp_normal.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);

    dp = imm_dp_new(&m->abc);
    file = fopen(TMPDIR "/dp_normal.imm", "rb");
    imm_dp_read(dp, file);
    fclose(file);

    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();
    struct imm_seq seq = imm_seq(imm_str(model_normal_str), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -65826.0106185297);

    imm_del(task);
    imm_del(&result);
    imm_del(dp);
    model_normal_deinit();
}

void test_dp_io_large_frame(void)
{
    model_frame_init();
    struct model_frame *m = &model_frame;
    struct imm_dp *dp = imm_hmm_new_dp(m->hmm, imm_super(m->end));

    FILE *file = fopen(TMPDIR "/dp_frame.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);

    struct imm_abc const *abc = imm_super(imm_super(m->dna));
    dp = imm_dp_new(abc);
    file = fopen(TMPDIR "/dp_frame.imm", "rb");
    imm_dp_read(dp, file);
    fclose(file);

    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();
    struct imm_seq seq = imm_seq(imm_str(model_frame_str), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -1622.8488101101);

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    model_frame_deinit();
}
