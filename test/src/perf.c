#include "cass/cass.h"
#include "helper.h"
#include "imm/imm.h"
#include "model2.h"
#include <stdio.h>
#include <stdlib.h>

void test_perf_viterbi(void);
void test_perf_viterbi_output(void);
void test_perf_viterbi_input(void);

int main(void)
{
    test_perf_viterbi();
    test_perf_viterbi_output();
    test_perf_viterbi_input();
    return cass_status();
}

void test_perf_viterbi(void)
{
    char const* str = get_model2_str();
    cass_cond(strlen(str) == 2000);

    struct model2        model = create_model2();
    struct imm_dp const* dp = imm_hmm_create_dp(model.hmm, imm_mute_state_super(model.end));

    struct imm_seq const* seq = imm_seq_create(str, model.abc);
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 0);
    struct imm_results const* results = imm_dp_viterbi(dp, task);
    imm_dp_task_destroy(task);

    cass_cond(imm_results_size(results) == 1);
    struct imm_result const* r = imm_results_get(results, 0);
    struct imm_subseq        subseq = imm_result_subseq(r);
    imm_float score = imm_hmm_loglikelihood(model.hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(imm_lprob_is_valid(score));
    cass_cond(!imm_lprob_is_zero(score));
    cass_close(score, -65826.0118484497);
    imm_results_destroy(results);

    imm_dp_destroy(dp);
    destroy_model2(model);
    imm_seq_destroy(seq);
}

void test_perf_viterbi_output(void)
{
    struct model2      model = create_model2();
    struct imm_output* output = imm_output_create(TMPDIR "/perf.imm");
    cass_cond(output != NULL);

    struct imm_dp const*    dp = imm_hmm_create_dp(model.hmm, imm_mute_state_super(model.end));
    struct imm_model const* imodel = imm_model_create(model.hmm, dp);

    cass_equal_int(imm_output_write(output, imodel), 0);
    imm_model_destroy(imodel);
    cass_equal_int(imm_output_destroy(output), 0);

    imm_dp_destroy(dp);
    destroy_model2(model);
}

void test_perf_viterbi_input(void)
{
    struct imm_input* input = imm_input_create(TMPDIR "/perf.imm");
    cass_cond(input != NULL);
    struct imm_model const* imodel = imm_input_read(input);
    cass_cond(imodel != NULL);
    cass_equal_int(imm_input_destroy(input), 0);

    struct imm_abc const* abc = imm_model_abc(imodel);
    struct imm_hmm const* hmm = imm_model_hmm(imodel);
    struct imm_dp const*  dp = imm_model_dp(imodel);

    char const* str = get_model2_str();
    cass_cond(strlen(str) == 2000);

    struct imm_seq const* seq = imm_seq_create(str, abc);
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq, 0);
    struct imm_results const* results = imm_dp_viterbi(dp, task);
    struct imm_result const*  r = imm_results_get(results, 0);
    struct imm_subseq         subseq = imm_result_subseq(r);
    imm_float score = imm_hmm_loglikelihood(hmm, imm_subseq_cast(&subseq), imm_result_path(r));
    cass_cond(imm_lprob_is_valid(score));
    cass_cond(!imm_lprob_is_zero(score));
    cass_close(score, -65826.0118484497);
    imm_results_destroy(results);

    for (uint16_t i = 0; i < imm_model_nstates(imodel); ++i)
        imm_state_destroy(imm_model_state(imodel, i));

    imm_dp_destroy(dp);
    imm_model_destroy(imodel);
    imm_abc_destroy(abc);
    imm_hmm_destroy(hmm);
    imm_seq_destroy(seq);
}
