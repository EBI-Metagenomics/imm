#include "cass/cass.h"
#include "imm/imm.h"
#include <stdlib.h>

#define CLOSE(a, b) cass_close2(a, b, 1e-6, 0.0)

void test_hmm_write_io_two_states(void);

int main(void)
{
    test_hmm_write_io_two_states();
    return cass_status();
}

static inline imm_float zero(void) { return imm_lprob_zero(); }

void test_hmm_write_io_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create("state0", abc);

    imm_float                      lprobs1[] = {logf(0.25f), logf(0.25f), logf(0.5), zero()};
    struct imm_normal_state const* state1 = imm_normal_state_create("state1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0), logf(0.5));
    imm_hmm_set_start(hmm, imm_mute_state_super(state0), logf(0.1f));
    imm_hmm_add_state(hmm, imm_normal_state_super(state1), logf(0.001f));
    imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_normal_state_super(state1), logf(0.9f));

    struct imm_dp const* dp = imm_hmm_create_dp(hmm, imm_normal_state_super(state1));

    struct imm_dp_task* task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, C, 0);
    struct imm_results const* results = imm_dp_viterbi(dp, task);
    cass_cond(results != NULL);
    cass_equal_int(imm_results_size(results), 1);
    struct imm_path const* path = imm_result_path(imm_results_get(results, 0));

    struct imm_result const* r = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(r);
    struct imm_seq const*    s = imm_subseq_cast(&subseq);
    imm_float                score = imm_hmm_likelihood(hmm, s, imm_result_path(r));
    CLOSE(score, logf(0.25f) + logf(0.1f) + logf(0.9f));
    CLOSE(imm_hmm_likelihood(hmm, C, path), logf(0.25f) + logf(0.1f) + logf(0.9f));
    imm_results_destroy(results);

    struct imm_output* output = imm_output_create(TMPDIR "/two_states.imm");
    cass_cond(output != NULL);
    struct imm_model const* model = imm_model_create(hmm, dp);
    cass_equal_int(imm_output_write(output, model), 0);
    imm_model_destroy(model);
    model = imm_model_create(hmm, dp);
    cass_equal_int(imm_output_write(output, model), 0);
    imm_model_destroy(model);
    cass_equal_int(imm_output_destroy(output), 0);

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
    model = imm_input_read(input);
    cass_cond(!imm_input_eof(input));
    cass_cond(model != NULL);

    cass_equal_uint64(imm_model_nstates(model), 2);

    abc = imm_model_abc(model);
    hmm = imm_model_hmm(model);
    dp = imm_model_dp(model);
    C = imm_seq_create("C", abc);

    task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, C, 0);
    results = imm_dp_viterbi(dp, task);
    cass_cond(results != NULL);
    cass_equal_int(imm_results_size(results), 1);
    r = imm_results_get(results, 0);
    subseq = imm_result_subseq(r);
    s = imm_subseq_cast(&subseq);
    path = imm_result_path(r);
    score = imm_hmm_likelihood(hmm, s, imm_result_path(r));
    CLOSE(score, logf(0.25f) + logf(0.1f) + logf(0.9f));
    CLOSE(imm_hmm_likelihood(hmm, C, path), logf(0.25f) + logf(0.1f) + logf(0.9f));
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_model_nstates(model); ++i) {
        struct imm_state const* state = imm_model_state(model, i);

        if (imm_state_type_id(state) == IMM_MUTE_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state0") == 0);
        } else if (imm_state_type_id(state) == IMM_NORMAL_STATE_TYPE_ID) {
            cass_cond(strcmp(imm_state_get_name(state), "state1") == 0);
        }
    }

    for (uint16_t i = 0; i < imm_model_nstates(model); ++i)
        imm_state_destroy(imm_model_state(model, i));

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
    cass_equal_int(imm_input_destroy(input), 0);

    abc = imm_model_abc(model);
    hmm = imm_model_hmm(model);
    dp = imm_model_dp(model);

    for (uint16_t i = 0; i < imm_model_nstates(model); ++i)
        imm_state_destroy(imm_model_state(model, i));

    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_dp_destroy(dp);
    imm_model_destroy(model);
}
