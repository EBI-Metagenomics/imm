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
    imm_dp_task_setup(task, seq);
    struct imm_result const* r = imm_dp_viterbi(dp, task);
    imm_dp_task_destroy(task);

    imm_float score = imm_hmm_loglikelihood(model.hmm, seq, imm_result_path(r));
    cass_cond(imm_lprob_is_valid(score));
    cass_cond(!imm_lprob_is_zero(score));
    cass_close(score, -65826.0118484497);
    imm_result_destroy(r);

    imm_dp_destroy(dp);
    destroy_model2(model);
    imm_seq_destroy(seq);
}

void test_perf_viterbi_output(void)
{
    struct model2      model = create_model2();
    struct imm_output* output = imm_output_create(TMPDIR "/perf.imm");
    cass_cond(output != NULL);

    struct imm_dp const* dp = imm_hmm_create_dp(model.hmm, imm_mute_state_super(model.end));
    struct imm_profile*  imodel = imm_profile_create(model.abc);
    imm_profile_append_model(imodel, imm_model_create(model.hmm, dp));

    cass_equal(imm_output_write(output, imodel), 0);
    imm_profile_destroy(imodel, false);
    cass_equal(imm_output_destroy(output), 0);

    imm_dp_destroy(dp);
    destroy_model2(model);
}

void test_perf_viterbi_input(void)
{
    struct imm_input* input = imm_input_create(TMPDIR "/perf.imm");
    cass_cond(input != NULL);
    struct imm_profile const* prof = imm_input_read(input);
    cass_cond(prof != NULL);
    cass_equal(imm_input_destroy(input), 0);

    struct imm_abc const*   abc = imm_profile_abc(prof);
    struct imm_model const* model = imm_profile_get_model(prof, 0);
    struct imm_hmm const*   hmm = imm_model_hmm(model);
    struct imm_dp const*    dp = imm_model_dp(model);

    char const* str = get_model2_str();
    cass_cond(strlen(str) == 2000);

    struct imm_seq const* seq = imm_seq_create(str, abc);
    struct imm_dp_task*   task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq);
    struct imm_result const* r = imm_dp_viterbi(dp, task);
    imm_float                score = imm_hmm_loglikelihood(hmm, seq, imm_result_path(r));
    cass_cond(imm_lprob_is_valid(score));
    cass_cond(!imm_lprob_is_zero(score));
    cass_close(score, -65826.0118484497);
    imm_result_destroy(r);
    imm_dp_task_destroy(task);
    imm_seq_destroy(seq);
    imm_profile_destroy(prof, true);
}
