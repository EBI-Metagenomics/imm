#include "cass/cass.h"
#include "helper.h"
#include "imm/imm.h"
#include <stdlib.h>

#define CLOSE(a, b) cass_close2(a, b, 1e-6, 0.0)

void test_hmm_write_io_two_states(void);
void test_hmm_write_io_two_hmms(void);

int main(void)
{
    test_hmm_write_io_two_states();
    test_hmm_write_io_two_hmms();
    return cass_status();
}

void test_hmm_write_io_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);

    imm_float lprobs1[] = {imm_log(0.25f), imm_log(0.25f), imm_log(0.5), zero()};
    struct imm_normal_state const* state1 = imm_normal_state_create("state1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0), imm_log(0.5));
    imm_hmm_set_start(hmm, imm_mute_state_super(state0), imm_log(0.1f));
    imm_hmm_add_state(hmm, imm_normal_state_super(state1), imm_log(0.001f));
    imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_normal_state_super(state1),
                      imm_log(0.9f));

    struct imm_dp const* dp = imm_hmm_create_dp(hmm, imm_normal_state_super(state1));

    struct imm_dp_task* task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, C, 0);
    struct imm_results const* results = imm_dp_viterbi(dp, task);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    struct imm_path const* path = imm_result_path(imm_results_get(results, 0));

    struct imm_result const* r = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(r);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_loglikelihood(hmm, s, imm_result_path(r));
    CLOSE(score, imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm, C, path), imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    struct imm_output* output = imm_output_create(TMPDIR "/two_states.imm");
    cass_cond(output != NULL);
    struct imm_model* m = imm_model_create(abc);
    imm_model_append_hmm_block(m, imm_hmm_block_create(hmm, dp));
    cass_equal(imm_output_write(output, m), 0);
    imm_model_destroy(m);
    m = imm_model_create(abc);
    imm_model_append_hmm_block(m, imm_hmm_block_create(hmm, dp));
    cass_equal(imm_output_write(output, m), 0);
    imm_model_destroy(m);
    cass_equal(imm_output_destroy(output), 0);

    imm_dp_destroy(dp);
    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);
    imm_dp_task_destroy(task);

    struct imm_input* input = imm_input_create(TMPDIR "/two_states.imm");
    cass_cond(input != NULL);
    cass_cond(!imm_input_eof(input));
    struct imm_model const* model = imm_input_read(input);
    cass_cond(!imm_input_eof(input));
    cass_cond(model != NULL);
    cass_equal(imm_model_nhmm_blocks(model), 1);

    struct imm_hmm_block* block = imm_model_get_hmm_block(model, 0);
    cass_cond(block != NULL);
    cass_equal_uint64(imm_hmm_block_nstates(block), 2);

    abc = imm_model_abc(model);
    hmm = imm_hmm_block_hmm(block);
    dp = imm_hmm_block_dp(block);
    C = imm_seq_create("C", abc);

    task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, C, 0);
    results = imm_dp_viterbi(dp, task);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    s = imm_subseq_cast(&subseq);
    path = imm_result_path(r);
    score = imm_hmm_loglikelihood(hmm, s, imm_result_path(r));
    CLOSE(score, imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm, C, path), imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block); ++i) {
        struct imm_state const* state = imm_hmm_block_state(block, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state0") == 0);
        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state1") == 0);
        }
    }

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block); ++i)
        imm_state_destroy(imm_hmm_block_state(block, i));

    imm_dp_task_destroy(task);
    imm_seq_destroy(C);
    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_model_destroy(model);

    model = imm_input_read(input);
    cass_cond(model != NULL);
    cass_cond(imm_input_read(input) == NULL);
    cass_cond(imm_input_eof(input));
    cass_equal(imm_input_destroy(input), 0);

    cass_equal(imm_model_nhmm_blocks(model), 1);
    block = imm_model_get_hmm_block(model, 0);
    cass_cond(block != NULL);

    abc = imm_model_abc(model);
    hmm = imm_hmm_block_hmm(block);
    dp = imm_hmm_block_dp(block);

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block); ++i)
        imm_state_destroy(imm_hmm_block_state(block, i));

    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_model_destroy(model);
}

void test_hmm_write_io_two_hmms(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm0 = imm_hmm_create(abc);
    struct imm_hmm*       hmm1 = imm_hmm_create(abc);

    struct imm_mute_state const* hmm0_state0 = imm_mute_state_create("hmm0_state0", abc);
    imm_float hmm0_lprobs[] = {imm_log(0.25f), imm_log(0.25f), imm_log(0.5), zero()};
    struct imm_normal_state const* hmm0_state1 =
        imm_normal_state_create("hmm0_state1", abc, hmm0_lprobs);
    imm_hmm_add_state(hmm0, imm_mute_state_super(hmm0_state0), imm_log(0.5));
    imm_hmm_set_start(hmm0, imm_mute_state_super(hmm0_state0), imm_log(0.1f));
    imm_hmm_add_state(hmm0, imm_normal_state_super(hmm0_state1), imm_log(0.001f));
    imm_hmm_set_trans(hmm0, imm_mute_state_super(hmm0_state0), imm_normal_state_super(hmm0_state1),
                      imm_log(0.9f));

    struct imm_mute_state const* hmm1_state0 = imm_mute_state_create("hmm1_state0", abc);
    imm_float hmm1_lprobs[] = {imm_log(0.05f), imm_log(0.05f), imm_log(0.1), zero()};
    struct imm_normal_state const* hmm1_state1 =
        imm_normal_state_create("hmm1_state1", abc, hmm1_lprobs);
    imm_hmm_add_state(hmm1, imm_mute_state_super(hmm1_state0), imm_log(0.5));
    imm_hmm_set_start(hmm1, imm_mute_state_super(hmm1_state0), imm_log(0.1f));
    imm_hmm_add_state(hmm1, imm_normal_state_super(hmm1_state1), imm_log(0.01f));
    imm_hmm_set_trans(hmm1, imm_mute_state_super(hmm1_state0), imm_normal_state_super(hmm1_state1),
                      imm_log(0.9f));

    struct imm_dp const* dp0 = imm_hmm_create_dp(hmm0, imm_normal_state_super(hmm0_state1));
    struct imm_dp const* dp1 = imm_hmm_create_dp(hmm1, imm_normal_state_super(hmm1_state1));

    struct imm_dp_task* task0 = imm_dp_task_create(dp0);
    imm_dp_task_setup(task0, C, 0);
    struct imm_results const* results = imm_dp_viterbi(dp0, task0);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    struct imm_path const*   path = imm_result_path(imm_results_get(results, 0));
    struct imm_result const* r = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(r);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_loglikelihood(hmm0, s, imm_result_path(r));
    CLOSE(score, imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm0, C, path), imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    struct imm_dp_task* task1 = imm_dp_task_create(dp1);
    imm_dp_task_setup(task1, C, 0);
    results = imm_dp_viterbi(dp1, task1);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    path = imm_result_path(imm_results_get(results, 0));
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    s = imm_subseq_cast(&subseq);
    score = imm_hmm_loglikelihood(hmm1, s, imm_result_path(r));
    CLOSE(score, imm_log(0.05f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm1, C, path), imm_log(0.05f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    struct imm_output* output = imm_output_create(TMPDIR "/two_hmms.imm");
    cass_cond(output != NULL);
    struct imm_model* m = imm_model_create(abc);
    imm_model_append_hmm_block(m, imm_hmm_block_create(hmm0, dp0));
    imm_model_append_hmm_block(m, imm_hmm_block_create(hmm1, dp1));
    cass_equal(imm_output_write(output, m), 0);
    imm_model_destroy(m);
    cass_equal(imm_output_destroy(output), 0);

    imm_dp_destroy(dp0);
    imm_dp_destroy(dp1);
    imm_hmm_destroy(hmm0);
    imm_hmm_destroy(hmm1);
    imm_mute_state_destroy(hmm0_state0);
    imm_normal_state_destroy(hmm0_state1);
    imm_mute_state_destroy(hmm1_state0);
    imm_normal_state_destroy(hmm1_state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);
    imm_dp_task_destroy(task0);
    imm_dp_task_destroy(task1);

    struct imm_input* input = imm_input_create(TMPDIR "/two_hmms.imm");
    cass_cond(input != NULL);
    cass_cond(!imm_input_eof(input));
    struct imm_model const* model = imm_input_read(input);
    cass_cond(!imm_input_eof(input));
    imm_input_destroy(input);
    cass_cond(model != NULL);

    struct imm_hmm_block* block0 = imm_model_get_hmm_block(model, 0);
    cass_cond(block0 != NULL);
    cass_equal(imm_hmm_block_nstates(block0), 2);

    struct imm_hmm_block* block1 = imm_model_get_hmm_block(model, 1);
    cass_cond(block1 != NULL);
    cass_equal(imm_hmm_block_nstates(block1), 2);

    abc = imm_model_abc(model);
    hmm0 = imm_hmm_block_hmm(block0);
    dp0 = imm_hmm_block_dp(block0);
    hmm1 = imm_hmm_block_hmm(block1);
    dp1 = imm_hmm_block_dp(block1);
    C = imm_seq_create("C", abc);

    task0 = imm_dp_task_create(dp0);
    imm_dp_task_setup(task0, C, 0);
    results = imm_dp_viterbi(dp0, task0);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    s = imm_subseq_cast(&subseq);
    path = imm_result_path(r);
    score = imm_hmm_loglikelihood(hmm0, s, imm_result_path(r));
    CLOSE(score, imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm0, C, path), imm_log(0.25f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block0); ++i) {
        struct imm_state const* state = imm_hmm_block_state(block0, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "hmm0_state0") == 0);
        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "hmm0_state1") == 0);
        }
    }

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block0); ++i)
        imm_state_destroy(imm_hmm_block_state(block0, i));

    task1 = imm_dp_task_create(dp1);
    imm_dp_task_setup(task1, C, 0);
    results = imm_dp_viterbi(dp1, task1);
    cass_cond(results != NULL);
    cass_equal(imm_results_size(results), 1);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    s = imm_subseq_cast(&subseq);
    path = imm_result_path(r);
    score = imm_hmm_loglikelihood(hmm1, s, imm_result_path(r));
    CLOSE(score, imm_log(0.05f) + imm_log(0.1f) + imm_log(0.9f));
    CLOSE(imm_hmm_loglikelihood(hmm1, C, path), imm_log(0.05f) + imm_log(0.1f) + imm_log(0.9f));
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block1); ++i) {
        struct imm_state const* state = imm_hmm_block_state(block1, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "hmm1_state0") == 0);
        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "hmm1_state1") == 0);
        }
    }

    for (uint16_t i = 0; i < imm_hmm_block_nstates(block1); ++i)
        imm_state_destroy(imm_hmm_block_state(block1, i));

    imm_dp_task_destroy(task0);
    imm_dp_task_destroy(task1);
    imm_seq_destroy(C);
    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm0);
    imm_dp_destroy(dp0);
    imm_hmm_destroy(hmm1);
    imm_dp_destroy(dp1);
    imm_model_destroy(model);
}
