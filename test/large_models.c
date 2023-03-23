#include "hope.h"
#include "imm/imm.h"

void test_large_normal(bool save_path);
void test_large_msv_normal(bool save_path);
void test_large_normal_nopath(void);
void test_large_normal_removed_states(bool save_path);
void test_large_frame(struct imm_span, imm_float loglik, bool save_path);

int main(void)
{
    test_large_normal_nopath();
    bool save_paths[2] = {true, false};
    for (int i = 0; i < 2; ++i)
    {
        bool x = save_paths[i];
        test_large_normal(x);
        test_large_msv_normal(x);
        test_large_normal_removed_states(x);
        test_large_frame(imm_span(1, 5), -1622.8488101101, x);
        test_large_frame(imm_span(2, 4), -1793.03239395872, x);
        test_large_frame(imm_span(3, 3), imm_lprob_nan(), x);
    }

    return hope_status();
}

void test_large_normal(bool save_path)
{
    imm_example1_init(IMM_EXAMPLE1_SIZE);

    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    imm_task_set_save_path(task, save_path);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -194581.04361377980);
    printf("Normal msecs: %llu\n", prod.mseconds);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_msv_normal(bool save_path)
{
    imm_msv_example1_init(IMM_MSV_EXAMPLE1_SIZE);

    struct imm_msv_example1 *m = &imm_msv_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_msv_example1.hmm, imm_super(&m->T), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(imm_str(imm_msv_example1_seq1), &m->abc);
    eq(imm_task_setup(task, &seq), IMM_OK);

    imm_task_set_save_path(task, save_path);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -6369.93095660004);
    printf("MSV        msecs: %llu\n", prod.mseconds);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_normal_nopath(void)
{
    imm_example1_init(IMM_EXAMPLE1_SIZE);

    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    imm_task_set_save_path(task, false);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    imm_task_set_save_path(task, true);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -194581.04361377980);
    printf("Normal-nopath msecs: %llu\n", prod.mseconds);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_normal_removed_states(bool save_path)
{
    imm_example1_init(IMM_EXAMPLE1_SIZE);
    imm_example1_remove_insertion_states(IMM_EXAMPLE1_SIZE);
    imm_example1_remove_deletion_states(IMM_EXAMPLE1_SIZE);

    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq =
        imm_seq(imm_str(imm_example1_seq_only_matches), &m->abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    imm_task_set_save_path(task, save_path);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -4833.14205103985);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_frame(struct imm_span span, imm_float loglik, bool save_path)
{
    imm_example2_init(span);

    struct imm_example2 *m = &imm_example2;
    struct imm_dp dp;
    imm_hmm_init_dp(&m->hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_nuclt const *nuclt = imm_super(m->dna);
    struct imm_abc const *abc = imm_super(nuclt);

    struct imm_seq seq = imm_seq(imm_str(imm_example2_seq), abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    imm_task_set_save_path(task, save_path);

    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, loglik);
    printf("Frame msecs: %llu\n", prod.mseconds);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}
