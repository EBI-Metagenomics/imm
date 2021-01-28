#include "cass/cass.h"
#include "imm/imm.h"
#include "model1.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char const* str = get_model1_str1();
    cass_cond(strlen(str) == 2000);

    unsigned ntasks = thread_max_size();
    printf("Number of tasks: %u\n", ntasks);

    struct model1        model = create_model1();
    struct imm_dp const* dp = imm_hmm_create_dp(model.hmm, imm_normal_state_super(model.end));

    struct imm_seq const** seqs = malloc(ntasks * sizeof(*seqs));
    struct imm_dp_task**   tasks = malloc(ntasks * sizeof(*tasks));
    for (unsigned i = 0; i < ntasks; ++i) {
        seqs[i] = imm_seq_create(str, model.abc);
        tasks[i] = imm_dp_task_create(dp);
    }

    unsigned num_seqs = 80;

#pragma omp parallel for
    for (unsigned i = 0; i < num_seqs; ++i) {
        struct imm_dp_task* task = tasks[thread_id()];
        imm_dp_task_setup(task, seqs[i % ntasks], 0);
        struct imm_results const* results = imm_dp_viterbi(dp, task);

        struct imm_result const* r = imm_results_get(results, 0);
        struct imm_subseq        subseq = imm_result_subseq(r);
        struct imm_path const*   p = imm_result_path(r);
        imm_float                score = imm_hmm_likelihood(model.hmm, imm_subseq_cast(&subseq), p);
        cass_cond(imm_lprob_is_valid(score));
        cass_cond(!imm_lprob_is_zero(score));
        cass_close(score, -10758.9710647332);
        cass_cond(imm_results_size(results) == 1);
        imm_results_destroy(results);
    }

    for (unsigned i = 0; i < ntasks; ++i) {
        imm_seq_destroy(seqs[i]);
        imm_dp_task_destroy(tasks[i]);
    }

    imm_dp_destroy(dp);
    destroy_model1(model);
    free(seqs);
    free(tasks);

    return cass_status();
}
