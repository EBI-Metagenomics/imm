#include "cass/cass.h"
#include "imm/error.h"
#include "imm/imm.h"

void test_dp(void);

int main(void)
{
    test_dp();
    return cass_status();
}

void test_dp(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_mute_state *state0 = imm_mute_state_new(0, abc);
    struct imm_mute_state *state1 = imm_mute_state_new(1, abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    cass_equal(imm_hmm_add_state(hmm, imm_super(state0)), IMM_SUCCESS);
    cass_equal(imm_hmm_add_state(hmm, imm_super(state1)), IMM_SUCCESS);

    cass_equal(imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1),
                                 imm_log(0.5)),
               IMM_SUCCESS);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state1));
    cass_null(dp);

    cass_equal(imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.3)),
               IMM_SUCCESS);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));

    struct imm_result result;
    imm_result_init(&result);

    struct imm_task *task = imm_task_new(dp);

    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);

    struct imm_seq const *seq = imm_seq_new(3, "ATT", abc);
    imm_task_setup(task, seq);

    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    /* cass_not_null(dp); */

    imm_del(task);
    imm_del(seq);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
}
