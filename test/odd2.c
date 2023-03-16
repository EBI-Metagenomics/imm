#include "hope.h"
#include "imm/imm.h"
#include <string.h>

enum state_id
{
    S_ID = 0,
    N_ID = 1,
    B_ID = 2,
    M1_ID = 3,
    J_ID = 6,
    E_ID = 7,
    C_ID = 8,
    T_ID = 9,
};

#define TPROB -1.5

static unsigned state_name(unsigned id, char *name);

int main(void)
{
    struct imm_abc abc = imm_abc_empty;
    eq(imm_abc_init(&abc, IMM_STR("C"), '*'), IMM_OK);

    struct imm_mute_state S = {0};
    struct imm_normal_state N = {0};
    struct imm_mute_state B = {0};

    struct imm_normal_state M1 = {0};

    struct imm_normal_state J = {0};

    struct imm_mute_state E = {0};
    struct imm_normal_state C = {0};
    struct imm_mute_state T = {0};

    imm_mute_state_init(&S, S_ID, &abc);
    imm_normal_state_init(&N, N_ID, &abc, IMM_ARR(-1));
    imm_mute_state_init(&B, B_ID, &abc);

    imm_normal_state_init(&M1, M1_ID, &abc, IMM_ARR(0));

    imm_normal_state_init(&J, J_ID, &abc, IMM_ARR(0));

    imm_mute_state_init(&E, E_ID, &abc);
    imm_normal_state_init(&C, C_ID, &abc, IMM_ARR(-1));
    imm_mute_state_init(&T, T_ID, &abc);

    static struct imm_code code = {0};
    imm_code_init(&code, &abc);
    struct imm_hmm hmm = {0};
    imm_hmm_init(&hmm, &code);

    imm_hmm_add_state(&hmm, imm_super(&S));
    imm_hmm_add_state(&hmm, imm_super(&N));
    imm_hmm_add_state(&hmm, imm_super(&B));

    imm_hmm_add_state(&hmm, imm_super(&M1));

    imm_hmm_add_state(&hmm, imm_super(&J));

    imm_hmm_add_state(&hmm, imm_super(&E));
    imm_hmm_add_state(&hmm, imm_super(&C));
    imm_hmm_add_state(&hmm, imm_super(&T));

    imm_hmm_set_trans(&hmm, imm_super(&S), imm_super(&N), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&N), imm_super(&N), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&N), imm_super(&B), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&S), imm_super(&B), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M1), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&E), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&B), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&J), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&J), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&B), TPROB);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&C), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&C), imm_super(&C), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&C), imm_super(&T), TPROB);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&T), TPROB);

    imm_hmm_set_start(&hmm, imm_super(&S), imm_log(1.0));

    FILE *fp = fopen(TMPDIR "/hmm.dot", "w");
    notnull(fp);
    imm_hmm_write_dot(&hmm, fp, &state_name);
    fclose(fp);

    struct imm_dp dp = {0};
    imm_hmm_init_dp(&hmm, imm_super(&T), &dp);
    struct imm_task *task = imm_task_new(&dp);
    struct imm_prod prod = imm_prod();

    struct imm_seq seq = imm_seq(IMM_STR("C"), &abc);
    // imm_task_setup(task, &seq);
    // eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    // eq(imm_path_nsteps(&prod.path), 5);
    // close(imm_hmm_loglik(&hmm, &seq, &prod.path), -15.39749512835);
    // close(prod.loglik, -15.39749512835);
    // imm_dp_dump_path(&dp, task, &prod, &state_name);

    // CCC TTT GGG
    seq = imm_seq(IMM_STR("C"), &abc);
    imm_task_setup(task, &seq);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 5);
    close(imm_hmm_loglik(&hmm, &seq, &prod.path), -6.00000000000);
    close(prod.loglik, -6.00000000000);
    imm_dp_dump_path(&dp, task, &prod, &state_name);

    imm_dp_write_dot(&dp, stdout, &state_name);
    // fp = fopen(TMPDIR "/dp.dot", "w");
    // notnull(fp);
    // imm_dp_write_dot(&dp, fp, &state_name);
    // fclose(fp);

    // imm_dp_dump_impl_details(&dp, stdout, &state_name);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);

    return hope_status();
}

static unsigned state_name(unsigned id, char *name)
{
    char *x = name;
    if (id == S_ID) *(x++) = 'S';
    if (id == N_ID) *(x++) = 'N';
    if (id == B_ID) *(x++) = 'B';

    if (id == M1_ID) *(x++) = 'M';
    if (id == M1_ID) *(x++) = '1';

    if (id == J_ID) *(x++) = 'J';

    if (id == E_ID) *(x++) = 'E';
    if (id == C_ID) *(x++) = 'C';
    if (id == T_ID) *(x++) = 'T';

    *x = '\0';
    return (unsigned)strlen(name);
}
