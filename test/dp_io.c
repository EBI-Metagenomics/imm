#include "hope/hope.h"
#include "imm/imm.h"
#include "model_frame.h"
#include "model_normal.h"

#define ZERO IMM_LPROB_ZERO

void test_dp_io(void);
void test_dp_io_large_normal(void);
void test_dp_io_large_frame(void);

int main(void)
{
    test_dp_io();
    test_dp_io_large_normal();
    test_dp_io_large_frame();
    return hope_status();
}

void test_dp_io(void)
{
    struct imm_abc abc;
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    struct imm_hmm *hmm = imm_hmm_new(&abc);
    struct imm_result result = imm_result();

    struct imm_seq A = imm_seq(IMM_STR("A"), &abc);
    struct imm_seq AG = imm_seq(IMM_STR("AG"), &abc);

    imm_float const lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                                 ZERO};
    struct imm_normal_state *state0 = imm_normal_state_new(0, &abc, lprobs0);

    imm_float const lprobs1[] = {
        imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),
        imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, &abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(1.0));

    imm_hmm_normalize_trans(hmm);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);

    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, &A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(result.loglik, -1.386294361120);

    EQ(imm_task_setup(task, &AG), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(result.loglik, -3.178053830348);

    imm_del(task);

    FILE *file = fopen(TMPDIR "/dp.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);

    struct imm_dp *dp_in = imm_dp_new(&abc);

    file = fopen(TMPDIR "/dp.imm", "rb");
    imm_dp_read(dp_in, file);
    fclose(file);

    task = imm_task_new(dp_in);

    EQ(imm_task_setup(task, &A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp_in, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(result.loglik, -1.386294361120);

    EQ(imm_task_setup(task, &AG), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp_in, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(result.loglik, -3.178053830348);

    imm_del(task);

    imm_deinit(&result);
    imm_del(dp_in);
}

void test_dp_io_large_normal(void)
{
    model_normal_init();
    struct model_normal *m = &model_normal;
    struct imm_dp *dp = imm_hmm_new_dp(model_normal.hmm, imm_super(m->end));

    FILE *file = fopen(TMPDIR "/dp.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);

    dp = imm_dp_new(&m->abc);
    file = fopen(TMPDIR "/dp.imm", "rb");
    imm_dp_read(dp, file);
    fclose(file);

    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();
    struct imm_seq seq = imm_seq(imm_str(model_normal_str), &m->abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -65826.0106185297);

    imm_del(task);
    imm_deinit(&result);
    imm_del(dp);
    model_normal_deinit();
}

void test_dp_io_large_frame(void)
{
    model_frame_init();
    struct model_frame *m = &model_frame;
    struct imm_dp *dp = imm_hmm_new_dp(m->hmm, imm_super(m->end));

    FILE *file = fopen(TMPDIR "/dp2.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);

    struct imm_abc const *abc = imm_super(imm_super(m->dna));
    dp = imm_dp_new(abc);
    file = fopen(TMPDIR "/dp2.imm", "rb");
    imm_dp_read(dp, file);
    fclose(file);

    struct imm_task *task = imm_task_new(dp);
    struct imm_result result = imm_result();
    struct imm_seq seq = imm_seq(imm_str(model_frame_str), abc);
    EQ(imm_task_setup(task, &seq), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    CLOSE(result.loglik, -1622.8488101101);

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    model_frame_deinit();
}
