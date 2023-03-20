#include "hope.h"
#include "imm/imm.h"

void test_large_normal(void);
void test_large_normal_nopath(void);
void test_large_normal_removed_states(void);
void test_large_frame(struct imm_span, imm_float loglik);

int main(void)
{
    // test_large_normal();
    test_large_normal_nopath();
    // test_large_normal_removed_states();
    // test_large_frame(imm_span(1, 5), -1622.8488101101);
    // test_large_frame(imm_span(2, 4), -1793.03239395872);
    // test_large_frame(imm_span(3, 3), imm_lprob_nan());
    return hope_status();
}

void test_large_normal(void)
{
    imm_example1_init(IMM_EXAMPLE1_SIZE);

    struct imm_example1 *m = &imm_example1;
    struct imm_dp dp;
    imm_hmm_init_dp(&imm_example1.hmm, imm_super(&m->end), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(imm_str(imm_example1_seq), &m->abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -194581.04361377980);
    printf("Normal msecs: %llu\n", prod.mseconds);

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
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -194581.04361377980);
    printf("Normal-nopath msecs: %llu\n", prod.mseconds);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_normal_removed_states(void)
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
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, -4833.14205103985);

    imm_del(task);
    imm_del(&prod);
    imm_del(&dp);
}

void test_large_frame(struct imm_span span, imm_float loglik)
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

    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    close(prod.loglik, loglik);
    printf("Frame msecs: %llu\n", prod.mseconds);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}
