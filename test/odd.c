#include "hope.h"
#include "imm/imm.h"

static unsigned state_name(unsigned id, char *name);

static void test_no_odd_check(void);
static void test_need_odd_check(void);

int main(void)
{
    test_no_odd_check();
    test_need_odd_check();
    return hope_status();
}

static void test_no_odd_check(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("XJ"), '*'), IMM_OK);

    struct imm_mute_state B = {0};
    imm_mute_state_init(&B, 0, &abc);

    struct imm_normal_state X = {0};
    imm_normal_state_init(&X, 1, &abc, IMM_ARR(1., imm_lprob_zero()));

    struct imm_mute_state E = {0};
    imm_mute_state_init(&E, 2, &abc);

    struct imm_code code = {0};
    imm_code_init(&code, &abc);
    struct imm_hmm hmm = {0};
    imm_hmm_init(&hmm, &code);

    imm_hmm_add_state(&hmm, imm_super(&B));
    imm_hmm_add_state(&hmm, imm_super(&X));
    imm_hmm_add_state(&hmm, imm_super(&E));

    imm_hmm_set_start(&hmm, imm_super(&B), 10.0);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&X), 100.);
    imm_hmm_set_trans(&hmm, imm_super(&X), imm_super(&E), 1000.);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&B), 10000.);

    struct imm_dp dp = {0};
    imm_hmm_init_dp(&hmm, imm_super(&E), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(IMM_STR("XX"), &abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 6);
    close(imm_hmm_loglik(&hmm, &seq, &prod.path), 12212.);
    close(prod.loglik, 12212.);
    imm_dp_dump_path(&dp, task, &prod, &state_name);
    imm_hmm_write_dot(&hmm, stdout, &state_name);
    imm_dp_write_dot(&dp, stdout, &state_name);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);
}

static void test_need_odd_check(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("XJ"), '*'), IMM_OK);

    struct imm_mute_state B = {0};
    imm_mute_state_init(&B, 0, &abc);

    struct imm_normal_state X = {0};
    imm_normal_state_init(&X, 1, &abc, IMM_ARR(0., imm_lprob_zero()));

    struct imm_mute_state E = {0};
    imm_mute_state_init(&E, 2, &abc);

    struct imm_normal_state J = {0};
    imm_normal_state_init(&J, 3, &abc, IMM_ARR(imm_lprob_zero(), 0.));

    struct imm_code code = {0};
    imm_code_init(&code, &abc);
    struct imm_hmm hmm = {0};
    imm_hmm_init(&hmm, &code);

    imm_hmm_add_state(&hmm, imm_super(&B));
    imm_hmm_add_state(&hmm, imm_super(&X));
    imm_hmm_add_state(&hmm, imm_super(&E));
    imm_hmm_add_state(&hmm, imm_super(&J));

    imm_hmm_set_start(&hmm, imm_super(&B), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&X), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&X), imm_super(&E), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&B), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&J), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&J), 0.);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&B), 0.);

    struct imm_dp dp = {0};
    imm_hmm_init_dp(&hmm, imm_super(&E), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(IMM_STR("XJX"), &abc);
    eq(imm_task_setup(task, &seq), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 7);
    close(imm_hmm_loglik(&hmm, &seq, &prod.path), 0);
    close(prod.loglik, 0);
    imm_dp_dump_path(&dp, task, &prod, &state_name);
    imm_hmm_write_dot(&hmm, stdout, &state_name);
    imm_dp_write_dot(&dp, stdout, &state_name);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);
}

static unsigned state_name(unsigned id, char *name)
{
    char *x = name;
    if (id == 0) *(x++) = 'B';
    if (id == 1) *(x++) = 'X';
    if (id == 2) *(x++) = 'E';
    if (id == 3) *(x++) = 'J';

    *x = '\0';
    return (unsigned)strlen(name);
}
