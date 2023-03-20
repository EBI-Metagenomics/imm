#include "hope.h"
#include "imm/imm.h"

void test_viterbi_one_mute_state(bool check_path);
void test_viterbi_two_mute_states(bool check_path);
void test_viterbi_mute_cycle(void);
void test_viterbi_one_normal_state(bool check_path);
void test_viterbi_two_normal_states(bool check_path);
void test_viterbi_normal_states(bool check_path);
void test_viterbi_profile1(bool check_path);
void test_viterbi_profile2(bool check_path);
void test_viterbi_profile_delete(bool check_path);
void test_viterbi_global_profile(bool check_path);
void test_viterbi_cycle_mute_ending(bool check_path);

static struct imm_abc abc;
static struct imm_code code;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq C;
static struct imm_seq G;
static struct imm_seq T;
static struct imm_seq AA;
static struct imm_seq AC;
static struct imm_seq AG;
static struct imm_seq AT;
static struct imm_seq ACT;
static struct imm_seq AGT;
static struct imm_seq ATT;
static struct imm_seq AGTC;
static struct imm_seq GA;
static struct imm_seq GT;
static struct imm_seq GTTTA;
static struct imm_seq GTTTAC;
static struct imm_seq GTTTACA;

static struct imm_abc abc_ab;
static struct imm_code code_ab;
static struct imm_seq EMPTY_ab;
static struct imm_seq A_ab;
static struct imm_seq B_ab;
static struct imm_seq AA_ab;
static struct imm_seq AB_ab;
static struct imm_seq AAB_ab;

int main(void)
{
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    imm_code_init(&code, &abc);
    EMPTY = imm_seq(IMM_STR(""), &abc);
    A = imm_seq(IMM_STR("A"), &abc);
    C = imm_seq(IMM_STR("C"), &abc);
    G = imm_seq(IMM_STR("G"), &abc);
    T = imm_seq(IMM_STR("T"), &abc);
    AA = imm_seq(IMM_STR("AA"), &abc);
    AC = imm_seq(IMM_STR("AC"), &abc);
    AG = imm_seq(IMM_STR("AG"), &abc);
    AT = imm_seq(IMM_STR("AT"), &abc);
    ACT = imm_seq(IMM_STR("ACT"), &abc);
    AGT = imm_seq(IMM_STR("AGT"), &abc);
    ATT = imm_seq(IMM_STR("ATT"), &abc);
    AGTC = imm_seq(IMM_STR("AGTC"), &abc);
    GA = imm_seq(IMM_STR("GA"), &abc);
    GT = imm_seq(IMM_STR("GT"), &abc);
    GTTTA = imm_seq(IMM_STR("GTTTA"), &abc);
    GTTTAC = imm_seq(IMM_STR("GTTTAC"), &abc);
    GTTTACA = imm_seq(IMM_STR("GTTTACA"), &abc);

    imm_abc_init(&abc_ab, IMM_STR("AB"), '*');
    imm_code_init(&code_ab, &abc_ab);
    EMPTY_ab = imm_seq(IMM_STR(""), &abc_ab);
    A_ab = imm_seq(IMM_STR("A"), &abc_ab);
    B_ab = imm_seq(IMM_STR("B"), &abc_ab);
    AA_ab = imm_seq(IMM_STR("AA"), &abc_ab);
    AB_ab = imm_seq(IMM_STR("AB"), &abc_ab);
    AAB_ab = imm_seq(IMM_STR("AAB"), &abc_ab);

    test_viterbi_mute_cycle();

    bool check_path[2] = {true, false};
    for (int i = 0; i < 2; ++i)
    {
        bool x = check_path[i];
        test_viterbi_one_mute_state(x);
        test_viterbi_two_mute_states(x);
        test_viterbi_one_normal_state(x);
        test_viterbi_two_normal_states(x);
        test_viterbi_normal_states(x);
        test_viterbi_profile1(x);
        test_viterbi_profile2(x);
        test_viterbi_profile_delete(x);
        test_viterbi_global_profile(x);
        test_viterbi_cycle_mute_ending(x);
    }
    return hope_status();
}

#define RUN(seq, status, llik)                                                 \
    do                                                                         \
    {                                                                          \
        eq(imm_task_setup(task, seq), status);                                 \
        eq(imm_dp_viterbi(&dp, task, &prod), status);                          \
        close(prod.loglik, (llik));                                            \
    } while (0);

#define CHECK_PATH(seq, llik, nsteps)                                          \
    do                                                                         \
    {                                                                          \
        close(imm_hmm_loglik(&hmm, seq, &prod.path), (llik));                  \
        eq(imm_path_nsteps(&prod.path), nsteps);                               \
    } while (0);

#define DP_RESET(end_state, status)                                            \
    do                                                                         \
    {                                                                          \
        imm_del(&dp);                                                          \
        imm_hmm_init_dp(&hmm, imm_super(end_state), &dp);                      \
        eq(imm_task_reset(task, &dp), status);                                 \
    } while (0);

static inline imm_float zero(void) { return imm_lprob_zero(); }

void test_viterbi_one_mute_state(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state state;
    imm_mute_state_init(&state, 0, &abc);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(0.5));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);
    imm_task_set_save_path(task, check_path);

    RUN(&EMPTY, IMM_OK, imm_log(0.5));
    if (check_path) CHECK_PATH(&EMPTY, imm_log(0.5), 1);

    RUN(&C, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&C, imm_lprob_nan(), 0);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);
}

void test_viterbi_two_mute_states(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);
    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 1, &abc);

    imm_hmm_add_state(&hmm, imm_super(&state0));
    imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(0.5));
    imm_hmm_add_state(&hmm, imm_super(&state1));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state0), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&EMPTY, IMM_OK, imm_log(0.5));
    if (check_path) CHECK_PATH(&EMPTY, imm_log(0.5), 1);

    eq(imm_hmm_set_start(&hmm, imm_super(&state1), imm_lprob_zero()),
       IMM_NON_FINITE_PROBABILITY);

    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                      imm_log(0.1));

    DP_RESET(&state1, IMM_OK);

    eq(imm_task_setup(task, &EMPTY), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 2);
    eq(imm_path_step(&prod.path, 0)->state_id, 0);
    eq(imm_path_step(&prod.path, 0)->seqlen, 0);
    eq(imm_path_step(&prod.path, 1)->state_id, 1);
    eq(imm_path_step(&prod.path, 1)->seqlen, 0);
    close(imm_hmm_loglik(&hmm, &EMPTY, &prod.path),
          imm_log(0.5) + imm_log(0.1));
    close(prod.loglik, imm_log(0.5) + imm_log(0.1));

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_mute_cycle(void)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);

    imm_hmm_add_state(&hmm, imm_super(&state0));
    imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(0.5));

    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 1, &abc);
    imm_hmm_add_state(&hmm, imm_super(&state1));

    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                      imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state0),
                      imm_log(0.2));

    struct imm_dp dp;
    eq(imm_hmm_init_dp(&hmm, imm_super(&state0), &dp), IMM_TSORT_MUTE_CYLES);
}

void test_viterbi_one_normal_state(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state state;
    imm_normal_state_init(&state, 0, &abc, lprobs0);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state), &dp);
    struct imm_task *task = imm_task_new(&dp);

    eq(imm_task_setup(task, &EMPTY), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_SEQ_TOO_SHORT);

    RUN(&A, IMM_OK, imm_log(1.0) + imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(1.0) + imm_log(0.25), 1);
    RUN(&T, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&T, imm_lprob_nan(), 0);
    RUN(&AC, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&AC, imm_lprob_nan(), 0);

    imm_hmm_set_trans(&hmm, imm_super(&state), imm_super(&state), imm_log(0.1));
    DP_RESET(&state, IMM_OK);

    RUN(&A, IMM_OK, imm_log(1.0) + imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(1.0) + imm_log(0.25), 1);
    RUN(&AA, IMM_OK, imm_log(0.1) + 2 * imm_log(0.25));
    if (check_path) CHECK_PATH(&AA, imm_log(0.1) + 2 * imm_log(0.25), 2);
    RUN(&ACT, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&ACT, imm_lprob_nan(), 0);

    imm_hmm_normalize_trans(&hmm);
    DP_RESET(&state, IMM_OK);

    RUN(&A, IMM_OK, imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(0.25), 1);
    RUN(&AA, IMM_OK, 2 * imm_log(0.25));
    if (check_path) CHECK_PATH(&AA, 2 * imm_log(0.25), 2);
    RUN(&ACT, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&ACT, imm_lprob_nan(), 0);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_two_normal_states(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state state0;
    imm_normal_state_init(&state0, 0, &abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_log(0.5)};
    struct imm_normal_state state1;
    imm_normal_state_init(&state1, 1, &abc, lprobs1);

    imm_hmm_add_state(&hmm, imm_super(&state0));
    imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(0.1));
    imm_hmm_add_state(&hmm, imm_super(&state1));
    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                      imm_log(0.3));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state0), &dp);
    struct imm_task *task = imm_task_new(&dp);

    eq(imm_task_setup(task, &EMPTY), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_SEQ_TOO_SHORT);
    eq(imm_path_nsteps(&prod.path), 0);
    close(imm_hmm_loglik(&hmm, &EMPTY, &prod.path), imm_lprob_nan());
    close(prod.loglik, imm_lprob_nan());

    RUN(&A, IMM_OK, imm_log(0.1) + imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(0.1) + imm_log(0.25), 1);
    RUN(&T, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&T, imm_lprob_nan(), 0);
    RUN(&AC, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&AC, imm_lprob_nan(), 0);

    DP_RESET(&state1, IMM_OK);
    imm_float des = imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5);
    RUN(&AT, IMM_OK, des);
    if (check_path) CHECK_PATH(&AT, des, 2);

    RUN(&ATT, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&ATT, imm_lprob_nan(), 0);

    imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state1),
                      imm_log(0.5));
    imm_hmm_set_start(&hmm, imm_super(&state1), imm_lprob_zero());

    DP_RESET(&state1, IMM_OK);
    des = imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + 3 * imm_log(0.5);
    RUN(&ATT, IMM_OK, des);
    if (check_path) CHECK_PATH(&ATT, des, 3);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_normal_states(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    imm_float const lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                                 zero()};
    struct imm_normal_state state0;
    imm_normal_state_init(&state0, 0, &abc, lprobs0);

    imm_float const lprobs1[] = {
        imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),
        imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state state1;
    imm_normal_state_init(&state1, 1, &abc, lprobs1);

    imm_hmm_add_state(&hmm, imm_super(&state0));
    imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(&state1));

    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state0),
                      imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                      imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state1),
                      imm_log(1.0));

    imm_hmm_normalize_trans(&hmm);

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&state0), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&A, IMM_OK, -1.386294361120);
    if (check_path) CHECK_PATH(&A, -1.386294361120, 1);
    RUN(&AG, IMM_OK, -3.178053830348);
    if (check_path) CHECK_PATH(&AG, -3.178053830348, 2);

    DP_RESET(&state1, IMM_OK);
    RUN(&AG, IMM_OK, -3.295836866004);
    if (check_path) CHECK_PATH(&AG, -3.295836866004, 2);

    DP_RESET(&state0, IMM_OK);
    RUN(&AGT, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&AGT, imm_lprob_nan(), 0);

    DP_RESET(&state1, IMM_OK);
    RUN(&AGT, IMM_OK, -4.106767082221);
    if (check_path) CHECK_PATH(&AGT, -4.106767082221, 3);

    DP_RESET(&state0, IMM_OK);
    RUN(&AGTC, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&AGTC, imm_lprob_nan(), 0);

    DP_RESET(&state1, IMM_OK);
    RUN(&AGTC, IMM_OK, -6.303991659557);
    if (check_path) CHECK_PATH(&AGTC, -6.303991659557, 4);

    eq(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state0), zero()),
       IMM_NON_FINITE_PROBABILITY);
    eq(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1), zero()),
       IMM_NON_FINITE_PROBABILITY);
    eq(imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state0), zero()),
       IMM_NON_FINITE_PROBABILITY);
    eq(imm_hmm_set_trans(&hmm, imm_super(&state1), imm_super(&state1), zero()),
       IMM_NON_FINITE_PROBABILITY);

    eq(imm_hmm_set_start(&hmm, imm_super(&state0), zero()),
       IMM_NON_FINITE_PROBABILITY);
    eq(imm_hmm_set_start(&hmm, imm_super(&state1), zero()),
       IMM_NON_FINITE_PROBABILITY);

    imm_hmm_set_start(&hmm, imm_super(&state0), imm_log(1.0));

    DP_RESET(&state0, IMM_OK);

    eq(imm_task_setup(task, &EMPTY), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_SEQ_TOO_SHORT);
    eq(imm_path_nsteps(&prod.path), 0);

    DP_RESET(&state1, IMM_OK);

    eq(imm_task_setup(task, &EMPTY), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_SEQ_TOO_SHORT);
    eq(imm_path_nsteps(&prod.path), 0);

    DP_RESET(&state0, IMM_OK);

    RUN(&A, IMM_OK, imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(0.25), 1);

    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state0),
                      imm_log(0.9));

    DP_RESET(&state0, IMM_OK);

    RUN(&A, IMM_OK, imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(0.25), 1);
    RUN(&AA, IMM_OK, 2 * imm_log(0.25) + imm_log(0.9));
    if (check_path) CHECK_PATH(&AA, 2 * imm_log(0.25) + imm_log(0.9), 2);

    imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                      imm_log(0.2));

    DP_RESET(&state0, IMM_OK);

    RUN(&A, IMM_OK, imm_log(0.25));
    if (check_path) CHECK_PATH(&A, imm_log(0.25), 1);
    RUN(&AA, IMM_OK, 2 * imm_log(0.25) + imm_log(0.9));
    if (check_path) CHECK_PATH(&AA, 2 * imm_log(0.25) + imm_log(0.9), 2);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_profile1(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code_ab);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state start;
    imm_mute_state_init(&start, 0, &abc_ab);
    struct imm_mute_state D0;
    imm_mute_state_init(&D0, 1, &abc_ab);
    struct imm_mute_state end;
    imm_mute_state_init(&end, 2, &abc_ab);

    imm_float M0_lprobs[] = {imm_log(0.4), imm_log(0.2)};
    struct imm_normal_state M0;
    imm_normal_state_init(&M0, 3, &abc_ab, M0_lprobs);

    imm_float I0_lprobs[] = {imm_log(0.5), imm_log(0.5)};
    struct imm_normal_state I0;
    imm_normal_state_init(&I0, 4, &abc_ab, I0_lprobs);

    imm_hmm_add_state(&hmm, imm_super(&start));
    imm_hmm_set_start(&hmm, imm_super(&start), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(&D0));
    imm_hmm_add_state(&hmm, imm_super(&end));

    imm_hmm_add_state(&hmm, imm_super(&M0));
    imm_hmm_add_state(&hmm, imm_super(&I0));

    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&D0), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&D0), imm_super(&end), imm_log(1.0));
    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&M0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&end), imm_log(0.8));
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&I0), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&I0), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&end), imm_log(1.0));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&end), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&EMPTY_ab, IMM_OK, imm_log(0.1) + imm_log(1.0));
    if (check_path) CHECK_PATH(&EMPTY_ab, imm_log(0.1) + imm_log(1.0), 3);

    DP_RESET(&D0, IMM_OK);
    RUN(&EMPTY_ab, IMM_OK, imm_log(0.1));
    if (check_path) CHECK_PATH(&EMPTY_ab, imm_log(0.1), 2);

    DP_RESET(&start, IMM_OK);
    RUN(&EMPTY_ab, IMM_OK, imm_log(1.0));
    if (check_path) CHECK_PATH(&EMPTY_ab, imm_log(1.0), 1);

    DP_RESET(&M0, IMM_OK);

    eq(imm_task_setup(task, &EMPTY_ab), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_SEQ_TOO_SHORT);
    eq(imm_path_nsteps(&prod.path), 0);
    close(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path), imm_lprob_nan());
    close(prod.loglik, imm_lprob_nan());

    DP_RESET(&M0, IMM_OK);
    imm_float des = imm_log(0.5) + imm_log(0.4);
    RUN(&A_ab, IMM_OK, des);
    if (check_path) CHECK_PATH(&A_ab, des, 2);

    DP_RESET(&end, IMM_OK);
    des = imm_log(0.5) + imm_log(0.4) + imm_log(0.8);
    RUN(&A_ab, IMM_OK, des);
    if (check_path) CHECK_PATH(&A_ab, des, 3);

    DP_RESET(&M0, IMM_OK);
    RUN(&B_ab, IMM_OK, imm_log(0.5) + imm_log(0.2));
    if (check_path) CHECK_PATH(&B_ab, imm_log(0.5) + imm_log(0.2), 2);

    DP_RESET(&end, IMM_OK);
    des = imm_log(0.5) + imm_log(0.2) + imm_log(0.8);
    RUN(&B_ab, IMM_OK, des);
    if (check_path) CHECK_PATH(&B_ab, des, 3);
    des = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5);
    RUN(&AA_ab, IMM_OK, des);
    if (check_path) CHECK_PATH(&AA_ab, des, 4);

    des = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) +
          2 * imm_log(0.5);
    RUN(&AAB_ab, IMM_OK, des);
    if (check_path) CHECK_PATH(&AAB_ab, des, 5);

    imm_del(&prod);
    imm_del(&dp);
    imm_del(task);
}

void test_viterbi_profile2(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state start;
    imm_mute_state_init(&start, 0, &abc);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1),
                              imm_log(0.7)};

    imm_float M0_lprobs[] = {imm_log(0.4), zero(), imm_log(0.6), zero()};
    imm_float M1_lprobs[] = {imm_log(0.6), zero(), imm_log(0.4), zero()};
    imm_float M2_lprobs[] = {imm_log(0.05), imm_log(0.05), imm_log(0.05),
                             imm_log(0.05)};

    struct imm_normal_state M0;
    imm_normal_state_init(&M0, 1, &abc, M0_lprobs);
    struct imm_normal_state I0;
    imm_normal_state_init(&I0, 2, &abc, ins_lprobs);

    struct imm_mute_state D1;
    imm_mute_state_init(&D1, 3, &abc);
    struct imm_normal_state M1;
    imm_normal_state_init(&M1, 4, &abc, M1_lprobs);
    struct imm_normal_state I1;
    imm_normal_state_init(&I1, 5, &abc, ins_lprobs);

    struct imm_mute_state D2;
    imm_mute_state_init(&D2, 6, &abc);
    struct imm_normal_state M2;
    imm_normal_state_init(&M2, 7, &abc, M2_lprobs);

    struct imm_mute_state end;
    imm_mute_state_init(&end, 8, &abc);

    imm_hmm_add_state(&hmm, imm_super(&start));
    imm_hmm_set_start(&hmm, imm_super(&start), 0.0);

    imm_hmm_add_state(&hmm, imm_super(&M0));
    imm_hmm_add_state(&hmm, imm_super(&I0));

    imm_hmm_add_state(&hmm, imm_super(&D1));
    imm_hmm_add_state(&hmm, imm_super(&M1));
    imm_hmm_add_state(&hmm, imm_super(&I1));

    imm_hmm_add_state(&hmm, imm_super(&D2));
    imm_hmm_add_state(&hmm, imm_super(&M2));

    imm_hmm_add_state(&hmm, imm_super(&end));

    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&M0), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&M1), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&M1), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&end), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&end), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M2), imm_super(&end), 0.0);

    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&I0), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&D1), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&M1), imm_log(0.5));

    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&D2), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&I1), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(&I1), imm_super(&I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I1), imm_super(&M2), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&D1), imm_super(&D2), imm_log(0.3));
    imm_hmm_set_trans(&hmm, imm_super(&D1), imm_super(&M2), imm_log(0.7));

    imm_hmm_set_trans(&hmm, imm_super(&D2), imm_super(&end), imm_log(1.0));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&M2), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&A, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&A, imm_log(0.05), 2);

    DP_RESET(&M2, IMM_OK);
    RUN(&C, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&C, imm_log(0.05), 2);
    RUN(&G, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&G, imm_log(0.05), 2);
    RUN(&T, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&T, imm_log(0.05), 2);

    DP_RESET(&end, IMM_OK);
    RUN(&A, IMM_OK, imm_log(0.6));
    if (check_path) CHECK_PATH(&A, imm_log(0.6), 3);
    RUN(&C, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&C, imm_log(0.05), 3);
    RUN(&G, IMM_OK, imm_log(0.6));
    if (check_path) CHECK_PATH(&G, imm_log(0.6), 3);
    RUN(&T, IMM_OK, imm_log(0.05));
    if (check_path) CHECK_PATH(&T, imm_log(0.05), 3);

    DP_RESET(&M1, IMM_OK);
    RUN(&A, IMM_OK, imm_log(0.6));
    if (check_path) CHECK_PATH(&A, imm_log(0.6), 2);

    DP_RESET(&end, IMM_OK);
    RUN(&GA, IMM_OK, 2 * imm_log(0.6));
    if (check_path) CHECK_PATH(&GA, 2 * imm_log(0.6), 4);

    DP_RESET(&I0, IMM_OK);
    RUN(&GT, IMM_OK, imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    if (check_path)
        CHECK_PATH(&GT, imm_log(0.6) + imm_log(0.2) + imm_log(0.7), 3);

    DP_RESET(&end, IMM_OK);
    imm_float des = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                    3 * imm_log(0.5) + imm_log(0.6);
    RUN(&GTTTA, IMM_OK, des);
    if (check_path) CHECK_PATH(&GTTTA, des, 7);

    des = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) +
          imm_log(0.6) + imm_log(0.05);
    RUN(&GTTTAC, IMM_OK, des);
    if (check_path) CHECK_PATH(&GTTTAC, des, 8);

    DP_RESET(&M2, IMM_OK);
    des = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) +
          imm_log(0.6) + imm_log(0.2) + imm_log(0.1) + imm_log(0.5) +
          imm_log(0.05);
    RUN(&GTTTACA, IMM_OK, des);
    if (check_path) CHECK_PATH(&GTTTACA, des, 8);

    DP_RESET(&M1, IMM_OK);
    des = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) + 3 * imm_log(0.7) +
          2 * imm_log(0.1) + imm_log(0.6);
    RUN(&GTTTACA, IMM_OK, des);
    if (check_path) CHECK_PATH(&GTTTACA, des, 8);

    DP_RESET(&end, IMM_OK);
    des = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) + 3 * imm_log(0.7) +
          2 * imm_log(0.1) + imm_log(0.6);
    RUN(&GTTTACA, IMM_OK, des);
    if (check_path) CHECK_PATH(&GTTTACA, des, 9);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_profile_delete(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code_ab);
    struct imm_prod prod = imm_prod();

    imm_float N0_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state N0;
    imm_normal_state_init(&N0, 0, &abc_ab, N0_lprobs);

    struct imm_mute_state M;
    imm_mute_state_init(&M, 1, &abc_ab);

    imm_float N1_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state N1;
    imm_normal_state_init(&N1, 2, &abc_ab, N1_lprobs);

    imm_float N2_lprobs[] = {zero(), imm_log(0.5)};
    struct imm_normal_state N2;
    imm_normal_state_init(&N2, 3, &abc_ab, N2_lprobs);

    imm_hmm_add_state(&hmm, imm_super(&N2));
    imm_hmm_add_state(&hmm, imm_super(&N1));
    imm_hmm_add_state(&hmm, imm_super(&M));
    imm_hmm_add_state(&hmm, imm_super(&N0));
    imm_hmm_set_start(&hmm, imm_super(&N0), 0);

    imm_hmm_set_trans(&hmm, imm_super(&N0), imm_super(&N1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&N0), imm_super(&M), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&N1), imm_super(&N2), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&M), imm_super(&N2), imm_log(0.5));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&N0), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&A_ab, IMM_OK, imm_log(0.5));
    if (check_path) CHECK_PATH(&A_ab, imm_log(0.5), 1);

    DP_RESET(&M, IMM_OK);
    RUN(&A_ab, IMM_OK, 2 * imm_log(0.5));
    if (check_path) CHECK_PATH(&A_ab, 2 * imm_log(0.5), 2);

    DP_RESET(&N2, IMM_OK);
    RUN(&AB_ab, IMM_OK, 4 * imm_log(0.5));
    if (check_path) CHECK_PATH(&AB_ab, 4 * imm_log(0.5), 3);

    DP_RESET(&M, IMM_OK);
    RUN(&A_ab, IMM_OK, 2 * imm_log(0.5));
    if (check_path) CHECK_PATH(&A_ab, 2 * imm_log(0.5), 2);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_global_profile(bool check_path)
{
    struct imm_abc abc_z;
    struct imm_code code_z;
    struct imm_seq AA_z;
    struct imm_seq AAB_z;
    struct imm_seq C_z;
    struct imm_seq CC_z;
    struct imm_seq CCC_z;
    struct imm_seq CCA_z;
    struct imm_seq CCAB_z;
    struct imm_seq CCABB_z;
    struct imm_seq CCABA_z;

    imm_abc_init(&abc_z, IMM_STR("ABCZ"), '*');
    imm_code_init(&code_z, &abc_z);
    AA_z = imm_seq(IMM_STR("AA"), &abc_z);
    AAB_z = imm_seq(IMM_STR("AAB"), &abc_z);
    C_z = imm_seq(IMM_STR("C"), &abc_z);
    CC_z = imm_seq(IMM_STR("CC"), &abc_z);
    CCC_z = imm_seq(IMM_STR("CCC"), &abc_z);
    CCA_z = imm_seq(IMM_STR("CCA"), &abc_z);
    CCAB_z = imm_seq(IMM_STR("CCAB"), &abc_z);
    CCABB_z = imm_seq(IMM_STR("CCABB"), &abc_z);
    CCABA_z = imm_seq(IMM_STR("CCABA"), &abc_z);

    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code_z);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state start;
    imm_mute_state_init(&start, 0, &abc_z);

    imm_float B_lprobs[] = {imm_log(0.01), imm_log(0.01), imm_log(1.0), zero()};
    struct imm_normal_state B;
    imm_normal_state_init(&B, 1, &abc_z, B_lprobs);

    imm_float M0_lprobs[] = {imm_log(0.9), imm_log(0.01), imm_log(0.01),
                             zero()};

    struct imm_normal_state M0;
    imm_normal_state_init(&M0, 2, &abc_z, M0_lprobs);

    imm_float M1_lprobs[] = {imm_log(0.01), imm_log(0.9), zero(), zero()};
    struct imm_normal_state M1;
    imm_normal_state_init(&M1, 3, &abc_z, M1_lprobs);

    imm_float M2_lprobs[] = {imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state M2;
    imm_normal_state_init(&M2, 4, &abc_z, M2_lprobs);

    struct imm_mute_state E;
    imm_mute_state_init(&E, 5, &abc_z);
    struct imm_mute_state end;
    imm_mute_state_init(&end, 6, &abc_z);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), imm_log(1.0)};
    struct imm_normal_state Z;
    imm_normal_state_init(&Z, 7, &abc_z, Z_lprobs);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), zero()};
    struct imm_normal_state I0;
    imm_normal_state_init(&I0, 8, &abc_z, ins_lprobs);
    struct imm_normal_state I1;
    imm_normal_state_init(&I1, 9, &abc_z, ins_lprobs);

    struct imm_mute_state D1;
    imm_mute_state_init(&D1, 10, &abc_z);
    struct imm_mute_state D2;
    imm_mute_state_init(&D2, 11, &abc_z);

    imm_hmm_add_state(&hmm, imm_super(&start));
    imm_hmm_set_start(&hmm, imm_super(&start), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(&B));
    imm_hmm_add_state(&hmm, imm_super(&M0));
    imm_hmm_add_state(&hmm, imm_super(&M1));
    imm_hmm_add_state(&hmm, imm_super(&M2));
    imm_hmm_add_state(&hmm, imm_super(&D1));
    imm_hmm_add_state(&hmm, imm_super(&D2));
    imm_hmm_add_state(&hmm, imm_super(&I0));
    imm_hmm_add_state(&hmm, imm_super(&I1));
    imm_hmm_add_state(&hmm, imm_super(&E));
    imm_hmm_add_state(&hmm, imm_super(&Z));
    imm_hmm_add_state(&hmm, imm_super(&end));

    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&B), 0);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&B), 0);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M0), 0);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M1), 0);
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&M1), 0);
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(&M2), imm_super(&E), 0);
    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&E), 0);
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&E), 0);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&end), 0);

    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&Z), 0);
    imm_hmm_set_trans(&hmm, imm_super(&Z), imm_super(&Z), 0);
    imm_hmm_set_trans(&hmm, imm_super(&Z), imm_super(&B), 0);

    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&D1), 0);
    imm_hmm_set_trans(&hmm, imm_super(&D1), imm_super(&D2), 0);
    imm_hmm_set_trans(&hmm, imm_super(&D1), imm_super(&M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(&D2), imm_super(&E), 0);

    imm_hmm_set_trans(&hmm, imm_super(&M0), imm_super(&I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I0), imm_super(&M1), imm_log(0.5));

    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I1), imm_super(&I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(&I1), imm_super(&M2), imm_log(0.5));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&start), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&C_z, IMM_OK, imm_lprob_nan());
    if (check_path) CHECK_PATH(&C_z, imm_lprob_nan(), 0);

    DP_RESET(&B, IMM_OK);
    RUN(&C_z, IMM_OK, imm_log(1.0));
    if (check_path) CHECK_PATH(&C_z, imm_log(1.0), 2);

    RUN(&CC_z, IMM_OK, imm_log(1.0));
    if (check_path) CHECK_PATH(&CC_z, imm_log(1.0), 3);
    RUN(&CCC_z, IMM_OK, imm_log(1.0));
    if (check_path) CHECK_PATH(&CCC_z, imm_log(1.0), 4);
    RUN(&CCA_z, IMM_OK, imm_log(0.01));
    if (check_path) CHECK_PATH(&CCA_z, imm_log(0.01), 4);

    DP_RESET(&M0, IMM_OK);
    RUN(&CCA_z, IMM_OK, imm_log(0.9));
    if (check_path) CHECK_PATH(&CCA_z, imm_log(0.9), 4);

    DP_RESET(&M1, IMM_OK);
    RUN(&CCAB_z, IMM_OK, 2 * imm_log(0.9));
    if (check_path) CHECK_PATH(&CCAB_z, 2 * imm_log(0.9), 5);

    DP_RESET(&I0, IMM_OK);
    RUN(&CCAB_z, IMM_OK, imm_log(0.9 * 0.5 * 0.1));
    if (check_path) CHECK_PATH(&CCAB_z, imm_log(0.9 * 0.5 * 0.1), 5);

    RUN(&CCABB_z, IMM_OK, imm_log(0.9) + 2 * (imm_log(0.05)));
    if (check_path) CHECK_PATH(&CCABB_z, imm_log(0.9) + 2 * (imm_log(0.05)), 6);

    DP_RESET(&M1, IMM_OK);
    imm_float des = imm_log(0.9) + imm_log(0.5) + imm_log(0.1) + imm_log(0.5) +
                    imm_log(0.01);
    RUN(&CCABA_z, IMM_OK, des);
    if (check_path) CHECK_PATH(&CCABA_z, des, 6);

    DP_RESET(&D1, IMM_OK);
    RUN(&AA_z, IMM_OK, imm_log(0.01) + imm_log(0.9));
    if (check_path) CHECK_PATH(&AA_z, imm_log(0.01) + imm_log(0.9), 4);

    DP_RESET(&D2, IMM_OK);
    RUN(&AA_z, IMM_OK, imm_log(0.01) + imm_log(0.9));
    if (check_path) CHECK_PATH(&AA_z, imm_log(0.01) + imm_log(0.9), 5);

    DP_RESET(&E, IMM_OK);
    RUN(&AA_z, IMM_OK, imm_log(0.01) + imm_log(0.9));
    if (check_path) CHECK_PATH(&AA_z, imm_log(0.01) + imm_log(0.9), 6);

    DP_RESET(&M2, IMM_OK);
    des = imm_log(0.01) + imm_log(0.9) + imm_log(0.5);
    RUN(&AAB_z, IMM_OK, des);
    if (check_path) CHECK_PATH(&AAB_z, des, 5);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}

void test_viterbi_cycle_mute_ending(bool check_path)
{
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code_ab);
    struct imm_prod prod = imm_prod();

    struct imm_mute_state start;
    imm_mute_state_init(&start, 0, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(&start));
    imm_hmm_set_start(&hmm, imm_super(&start), imm_log(1.0));

    struct imm_mute_state B;
    imm_mute_state_init(&B, 1, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(&B));

    imm_float lprobs[] = {imm_log(0.01), imm_log(0.01)};
    struct imm_normal_state M;
    imm_normal_state_init(&M, 2, &abc_ab, lprobs);
    imm_hmm_add_state(&hmm, imm_super(&M));

    struct imm_mute_state E;
    imm_mute_state_init(&E, 3, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(&E));

    struct imm_mute_state J;
    imm_mute_state_init(&J, 4, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(&J));

    struct imm_mute_state end;
    imm_mute_state_init(&end, 5, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(&end));

    imm_hmm_set_trans(&hmm, imm_super(&start), imm_super(&B), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&B), imm_super(&M), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&M), imm_super(&E), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&end), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&J), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(&J), imm_super(&B), imm_log(0.1));

    struct imm_dp dp;
    imm_hmm_init_dp(&hmm, imm_super(&end), &dp);
    struct imm_task *task = imm_task_new(&dp);

    RUN(&A_ab, IMM_OK, -13.815510557964272);
    if (check_path) CHECK_PATH(&A_ab, -13.815510557964272, 5);

    unsigned BM = imm_dp_trans_idx(&dp, imm_state_idx(imm_super(&B)),
                                   imm_state_idx(imm_super(&M)));

    imm_dp_change_trans(&dp, BM, imm_log(1.0));

    eq(imm_task_setup(task, &A_ab), IMM_OK);
    eq(imm_dp_viterbi(&dp, task, &prod), IMM_OK);
    eq(imm_path_nsteps(&prod.path), 5);
    close(prod.loglik, -11.5129254650);

    imm_del(&prod);
    imm_del(task);
    imm_del(&dp);
}
