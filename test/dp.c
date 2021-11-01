#include "hope/hope.h"
#include "imm/imm.h"

void test_dp_illegal(void);
void test_dp_empty_path(void);
void test_dp_one_mute(void);
void test_dp_two_mutes(void);

static struct imm_abc abc;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq ATT;

int main(void)
{
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    EMPTY = imm_seq(IMM_STR(""), &abc);
    A = imm_seq(IMM_STR("A"), &abc);
    ATT = imm_seq(IMM_STR("ATT"), &abc);

    test_dp_illegal();
    test_dp_empty_path();
    test_dp_one_mute();
    test_dp_two_mutes();
    return hope_status();
}

void test_dp_illegal(void)
{
    struct imm_mute_state state;
    imm_mute_state_init(&state, 3, &abc);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &abc);

    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_ILLEGALARG);
    imm_del(&dp);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_SUCCESS);
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_ILLEGALARG);
    imm_del(&dp);

    EQ(imm_hmm_set_start(&hmm, imm_super(&state), imm_log(0.1)), IMM_SUCCESS);
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_SUCCESS);
    imm_del(&dp);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state), imm_super(&state),
                         imm_log(0.5)),
       IMM_SUCCESS);
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_RUNTIMEERROR);
    imm_del(&dp);
}

void test_dp_empty_path(void)
{
    struct imm_mute_state state;
    imm_mute_state_init(&state, 3, &abc);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &abc);
    struct imm_prod prod = imm_prod();

    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_SUCCESS);
    EQ(imm_hmm_set_start(&hmm, imm_super(&state), imm_log(0.1)), IMM_SUCCESS);
    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_SUCCESS);

    struct imm_task *task = imm_task_new(&dp);
    imm_task_setup(task, &A);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    EQ(imm_path_nsteps(&prod.path), 0);

    imm_del(task);
    imm_del(&dp);
}

void test_dp_one_mute(void)
{
    struct imm_mute_state state;
    imm_mute_state_init(&state, 3, &abc);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &abc);
    struct imm_prod prod = imm_prod();

    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_SUCCESS);

    EQ(imm_hmm_set_start(&hmm, imm_super(&state), imm_log(0.3)), IMM_SUCCESS);
    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state), &dp), IMM_SUCCESS);

    struct imm_task *task = imm_task_new(&dp);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_ILLEGALARG);

    imm_task_setup(task, &EMPTY);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    EQ(imm_path_nsteps(&prod.path), 1);

    imm_task_setup(task, &ATT);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    EQ(imm_path_nsteps(&prod.path), 0);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_dp_two_mutes(void)
{
    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);
    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 12, &abc);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &abc);
    struct imm_prod prod = imm_prod();

    EQ(imm_hmm_add_state(&hmm, imm_super(&state0)), IMM_SUCCESS);
    EQ(imm_hmm_add_state(&hmm, imm_super(&state1)), IMM_SUCCESS);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                         imm_log(0.5)),
       IMM_SUCCESS);

    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state1), &dp), IMM_ILLEGALARG);

    EQ(imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(0.3)), IMM_SUCCESS);
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state1), &dp), IMM_SUCCESS);

    struct imm_task *task = imm_task_new(&dp);

    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_ILLEGALARG);

    imm_task_setup(task, &EMPTY);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    EQ(imm_path_nsteps(&prod.path), 2);
    EQ(imm_path_step(&prod.path, 0)->seqlen, 0);
    EQ(imm_path_step(&prod.path, 0)->state_id, 0);
    EQ(imm_path_step(&prod.path, 1)->seqlen, 0);
    EQ(imm_path_step(&prod.path, 1)->state_id, 12);

    imm_task_setup(task, &ATT);
    EQ(imm_dp_viterbi(&dp, task, &prod), IMM_SUCCESS);
    EQ(imm_path_nsteps(&prod.path), 0);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);
}
