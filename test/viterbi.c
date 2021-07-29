#include "hope/hope.h"
#include "imm/imm.h"

void test_viterbi_one_mute_state(void);
void test_viterbi_two_mute_states(void);
void test_viterbi_mute_cycle(void);
void test_viterbi_one_normal_state(void);
void test_viterbi_two_normal_states(void);
void test_viterbi_normal_states(void);
void test_viterbi_profile1(void);
void test_viterbi_profile2(void);
void test_viterbi_profile_delete(void);
void test_viterbi_global_profile(void);
void test_viterbi_cycle_mute_ending(void);

static struct imm_abc abc;
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
static struct imm_seq EMPTY_ab;
static struct imm_seq A_ab;
static struct imm_seq B_ab;
static struct imm_seq AA_ab;
static struct imm_seq AB_ab;
static struct imm_seq AAB_ab;

int main(void)
{
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
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
    EMPTY_ab = imm_seq(IMM_STR(""), &abc_ab);
    A_ab = imm_seq(IMM_STR("A"), &abc_ab);
    B_ab = imm_seq(IMM_STR("B"), &abc_ab);
    AA_ab = imm_seq(IMM_STR("AA"), &abc_ab);
    AB_ab = imm_seq(IMM_STR("AB"), &abc_ab);
    AAB_ab = imm_seq(IMM_STR("AAB"), &abc_ab);

    test_viterbi_one_mute_state();
    test_viterbi_two_mute_states();
    test_viterbi_mute_cycle();
    test_viterbi_one_normal_state();
    test_viterbi_two_normal_states();
    test_viterbi_normal_states();
    test_viterbi_profile1();
    test_viterbi_profile2();
    test_viterbi_profile_delete();
    test_viterbi_global_profile();
    test_viterbi_cycle_mute_ending();
    return hope_status();
}

#define VITERBI_CHECK(seq, status, nsteps, llik)                               \
    EQ(imm_task_setup(task, seq), status);                                     \
    EQ(imm_dp_viterbi(dp, task, &result), status);                             \
    EQ(imm_path_nsteps(&result.path), nsteps);                                 \
    CLOSE(imm_hmm_loglik(&hmm, seq, &result.path), (llik));                    \
    CLOSE(result.loglik, (llik));

#define DP_RESET(end_state, status)                                            \
    imm_del(dp);                                                               \
    dp = imm_hmm_new_dp(&hmm, imm_super(end_state));                           \
    EQ(imm_task_reset(task, dp), status);

static inline imm_float zero(void) { return imm_lprob_zero(); }

void test_viterbi_one_mute_state(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    struct imm_mute_state *state = imm_mute_state_new(0, &abc);

    imm_hmm_add_state(&hmm, imm_super(state));
    imm_hmm_set_start(&hmm, imm_super(state), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&EMPTY, IMM_SUCCESS, 1, imm_log(0.5));
    VITERBI_CHECK(&C, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_del(&result);
    imm_del(dp);
    imm_del(task);
    imm_del(state);
}

void test_viterbi_two_mute_states(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    struct imm_mute_state *state0 = imm_mute_state_new(0, &abc);
    struct imm_mute_state *state1 = imm_mute_state_new(1, &abc);

    imm_hmm_add_state(&hmm, imm_super(state0));
    imm_hmm_set_start(&hmm, imm_super(state0), imm_log(0.5));
    imm_hmm_add_state(&hmm, imm_super(state1));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&EMPTY, IMM_SUCCESS, 1, imm_log(0.5));

    EQ(imm_hmm_set_start(&hmm, imm_super(state1), imm_lprob_zero()),
       IMM_ILLEGALARG);

    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), imm_log(0.1));

    DP_RESET(state1, IMM_SUCCESS);

    EQ(imm_task_setup(task, &EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    EQ(imm_path_step(&result.path, 0)->state_id, 0);
    EQ(imm_path_step(&result.path, 0)->seqlen, 0);
    EQ(imm_path_step(&result.path, 1)->state_id, 1);
    EQ(imm_path_step(&result.path, 1)->seqlen, 0);
    CLOSE(imm_hmm_loglik(&hmm, &EMPTY, &result.path),
          imm_log(0.5) + imm_log(0.1));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.1));

    imm_del(&result);
    imm_del(state0);
    imm_del(state1);
    imm_del(task);
    imm_del(dp);
}

void test_viterbi_mute_cycle(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);

    struct imm_mute_state *state0 = imm_mute_state_new(0, &abc);

    imm_hmm_add_state(&hmm, imm_super(state0));
    imm_hmm_set_start(&hmm, imm_super(state0), imm_log(0.5));

    struct imm_mute_state *state1 = imm_mute_state_new(1, &abc);
    imm_hmm_add_state(&hmm, imm_super(state1));

    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(state1), imm_super(state0), imm_log(0.2));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state0));
    ISNULL(dp);

    imm_del(state0);
    imm_del(state1);
}

void test_viterbi_one_normal_state(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state = imm_normal_state_new(0, &abc, lprobs0);

    imm_hmm_add_state(&hmm, imm_super(state));
    imm_hmm_set_start(&hmm, imm_super(state), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, &EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(1.0) + imm_log(0.25));
    VITERBI_CHECK(&T, IMM_SUCCESS, 0, imm_lprob_nan());
    VITERBI_CHECK(&AC, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_set_trans(&hmm, imm_super(state), imm_super(state), imm_log(0.1));
    DP_RESET(state, IMM_SUCCESS);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(1.0) + imm_log(0.25));
    VITERBI_CHECK(&AA, IMM_SUCCESS, 2, imm_log(0.1) + 2 * imm_log(0.25));
    VITERBI_CHECK(&ACT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_normalize_trans(&hmm);
    DP_RESET(state, IMM_SUCCESS);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(&AA, IMM_SUCCESS, 2, 2 * imm_log(0.25));
    VITERBI_CHECK(&ACT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(state);
}

void test_viterbi_two_normal_states(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, &abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_log(0.5)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, &abc, lprobs1);

    imm_hmm_add_state(&hmm, imm_super(state0));
    imm_hmm_set_start(&hmm, imm_super(state0), imm_log(0.1));
    imm_hmm_add_state(&hmm, imm_super(state1));
    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), imm_log(0.3));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, &EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(&hmm, &EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(0.1) + imm_log(0.25));
    VITERBI_CHECK(&T, IMM_SUCCESS, 0, imm_lprob_nan());
    VITERBI_CHECK(&AC, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(&AT, IMM_SUCCESS, 2,
                  imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));

    VITERBI_CHECK(&ATT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_set_trans(&hmm, imm_super(state1), imm_super(state1), imm_log(0.5));
    imm_hmm_set_start(&hmm, imm_super(state1), imm_lprob_zero());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(&ATT, IMM_SUCCESS, 3,
                  imm_log(0.1) + imm_log(0.25) + imm_log(0.3) +
                      3 * imm_log(0.5));

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(state0);
    imm_del(state1);
}

void test_viterbi_normal_states(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    imm_float const lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                                 zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, &abc, lprobs0);

    imm_float const lprobs1[] = {
        imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),
        imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, &abc, lprobs1);

    imm_hmm_add_state(&hmm, imm_super(state0));
    imm_hmm_set_start(&hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(state1));

    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(state1), imm_super(state1), imm_log(1.0));

    imm_hmm_normalize_trans(&hmm);

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, -1.386294361120);
    VITERBI_CHECK(&AG, IMM_SUCCESS, 2, -3.178053830348);

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(&AG, IMM_SUCCESS, 2, -3.295836866004);

    DP_RESET(state0, IMM_SUCCESS);
    VITERBI_CHECK(&AGT, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(&AGT, IMM_SUCCESS, 3, -4.106767082221);

    DP_RESET(state0, IMM_SUCCESS);
    VITERBI_CHECK(&AGTC, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(&AGTC, IMM_SUCCESS, 4, -6.303991659557);

    EQ(imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state0), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(&hmm, imm_super(state1), imm_super(state0), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(&hmm, imm_super(state1), imm_super(state1), zero()),
       IMM_ILLEGALARG);

    EQ(imm_hmm_set_start(&hmm, imm_super(state0), zero()), IMM_ILLEGALARG);
    EQ(imm_hmm_set_start(&hmm, imm_super(state1), zero()), IMM_ILLEGALARG);

    imm_hmm_set_start(&hmm, imm_super(state0), imm_log(1.0));

    DP_RESET(state0, IMM_SUCCESS);

    EQ(imm_task_setup(task, &EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    DP_RESET(state1, IMM_SUCCESS);

    EQ(imm_task_setup(task, &EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(0.25));

    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state0), imm_log(0.9));

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(&AA, IMM_SUCCESS, 2, 2 * imm_log(0.25) + imm_log(0.9));

    imm_hmm_set_trans(&hmm, imm_super(state0), imm_super(state1), imm_log(0.2));

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(&A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(&AA, IMM_SUCCESS, 2, 2 * imm_log(0.25) + imm_log(0.9));

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(state0);
    imm_del(state1);
}

void test_viterbi_profile1(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc_ab);
    struct imm_result result = imm_result();

    struct imm_mute_state *start = imm_mute_state_new(0, &abc_ab);
    struct imm_mute_state *D0 = imm_mute_state_new(1, &abc_ab);
    struct imm_mute_state *end = imm_mute_state_new(2, &abc_ab);

    imm_float M0_lprobs[] = {imm_log(0.4), imm_log(0.2)};
    struct imm_normal_state *M0 = imm_normal_state_new(3, &abc_ab, M0_lprobs);

    imm_float I0_lprobs[] = {imm_log(0.5), imm_log(0.5)};
    struct imm_normal_state *I0 = imm_normal_state_new(4, &abc_ab, I0_lprobs);

    imm_hmm_add_state(&hmm, imm_super(start));
    imm_hmm_set_start(&hmm, imm_super(start), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(D0));
    imm_hmm_add_state(&hmm, imm_super(end));

    imm_hmm_add_state(&hmm, imm_super(M0));
    imm_hmm_add_state(&hmm, imm_super(I0));

    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(D0), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(D0), imm_super(end), imm_log(1.0));
    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(M0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(end), imm_log(0.8));
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(I0), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(end), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&EMPTY_ab, IMM_SUCCESS, 3, imm_log(0.1) + imm_log(1.0));

    DP_RESET(D0, IMM_SUCCESS);
    VITERBI_CHECK(&EMPTY_ab, IMM_SUCCESS, 2, imm_log(0.1));

    DP_RESET(start, IMM_SUCCESS);
    VITERBI_CHECK(&EMPTY_ab, IMM_SUCCESS, 1, imm_log(1.0));

    DP_RESET(M0, IMM_SUCCESS);

    EQ(imm_task_setup(task, &EMPTY_ab), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(&hmm, &EMPTY_ab, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 2, imm_log(0.5) + imm_log(0.4));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 3,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.8));

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(&B_ab, IMM_SUCCESS, 2, imm_log(0.5) + imm_log(0.2));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&B_ab, IMM_SUCCESS, 3,
                  imm_log(0.5) + imm_log(0.2) + imm_log(0.8));
    VITERBI_CHECK(&AA_ab, IMM_SUCCESS, 4,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5));

    VITERBI_CHECK(&AAB_ab, IMM_SUCCESS, 5,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) +
                      2 * imm_log(0.5));

    imm_del(&result);
    imm_del(dp);
    imm_del(task);
    imm_del(start);
    imm_del(D0);
    imm_del(M0);
    imm_del(I0);
    imm_del(end);
}

void test_viterbi_profile2(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc);
    struct imm_result result = imm_result();

    struct imm_mute_state *start = imm_mute_state_new(0, &abc);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1),
                              imm_log(0.7)};

    imm_float M0_lprobs[] = {imm_log(0.4), zero(), imm_log(0.6), zero()};
    imm_float M1_lprobs[] = {imm_log(0.6), zero(), imm_log(0.4), zero()};
    imm_float M2_lprobs[] = {imm_log(0.05), imm_log(0.05), imm_log(0.05),
                             imm_log(0.05)};

    struct imm_normal_state *M0 = imm_normal_state_new(1, &abc, M0_lprobs);
    struct imm_normal_state *I0 = imm_normal_state_new(2, &abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(3, &abc);
    struct imm_normal_state *M1 = imm_normal_state_new(4, &abc, M1_lprobs);
    struct imm_normal_state *I1 = imm_normal_state_new(5, &abc, ins_lprobs);

    struct imm_mute_state *D2 = imm_mute_state_new(6, &abc);
    struct imm_normal_state *M2 = imm_normal_state_new(7, &abc, M2_lprobs);

    struct imm_mute_state *end = imm_mute_state_new(8, &abc);

    imm_hmm_add_state(&hmm, imm_super(start));
    imm_hmm_set_start(&hmm, imm_super(start), 0.0);

    imm_hmm_add_state(&hmm, imm_super(M0));
    imm_hmm_add_state(&hmm, imm_super(I0));

    imm_hmm_add_state(&hmm, imm_super(D1));
    imm_hmm_add_state(&hmm, imm_super(M1));
    imm_hmm_add_state(&hmm, imm_super(I1));

    imm_hmm_add_state(&hmm, imm_super(D2));
    imm_hmm_add_state(&hmm, imm_super(M2));

    imm_hmm_add_state(&hmm, imm_super(end));

    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(M0), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(M1), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(M1), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(end), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(end), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(M2), imm_super(end), 0.0);

    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(D1), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(D2), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(I1), imm_log(0.2));
    imm_hmm_set_trans(&hmm, imm_super(I1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I1), imm_super(M2), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(D1), imm_super(D2), imm_log(0.3));
    imm_hmm_set_trans(&hmm, imm_super(D1), imm_super(M2), imm_log(0.7));

    imm_hmm_set_trans(&hmm, imm_super(D2), imm_super(end), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(M2));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&A, IMM_SUCCESS, 2, imm_log(0.05));

    DP_RESET(M2, IMM_SUCCESS);
    VITERBI_CHECK(&C, IMM_SUCCESS, 2, imm_log(0.05));
    VITERBI_CHECK(&G, IMM_SUCCESS, 2, imm_log(0.05));
    VITERBI_CHECK(&T, IMM_SUCCESS, 2, imm_log(0.05));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&A, IMM_SUCCESS, 3, imm_log(0.6));
    VITERBI_CHECK(&C, IMM_SUCCESS, 3, imm_log(0.05));
    VITERBI_CHECK(&G, IMM_SUCCESS, 3, imm_log(0.6));
    VITERBI_CHECK(&T, IMM_SUCCESS, 3, imm_log(0.05));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(&A, IMM_SUCCESS, 2, imm_log(0.6));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&GA, IMM_SUCCESS, 4, 2 * imm_log(0.6));

    DP_RESET(I0, IMM_SUCCESS);
    VITERBI_CHECK(&GT, IMM_SUCCESS, 3,
                  imm_log(0.6) + imm_log(0.2) + imm_log(0.7));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&GTTTA, IMM_SUCCESS, 7,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6));

    VITERBI_CHECK(&GTTTAC, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.05));

    DP_RESET(M2, IMM_SUCCESS);
    VITERBI_CHECK(&GTTTACA, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.2) +
                      imm_log(0.1) + imm_log(0.5) + imm_log(0.05));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(&GTTTACA, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
                      3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(&GTTTACA, IMM_SUCCESS, 9,
                  imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
                      3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6));

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(start);
    imm_del(M0);
    imm_del(I0);
    imm_del(D1);
    imm_del(M1);
    imm_del(I1);
    imm_del(D2);
    imm_del(M2);
    imm_del(end);
}

void test_viterbi_profile_delete(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc_ab);
    struct imm_result result = imm_result();

    imm_float N0_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N0 = imm_normal_state_new(0, &abc_ab, N0_lprobs);

    struct imm_mute_state *M = imm_mute_state_new(1, &abc_ab);

    imm_float N1_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N1 = imm_normal_state_new(2, &abc_ab, N1_lprobs);

    imm_float N2_lprobs[] = {zero(), imm_log(0.5)};
    struct imm_normal_state *N2 = imm_normal_state_new(3, &abc_ab, N2_lprobs);

    imm_hmm_add_state(&hmm, imm_super(N2));
    imm_hmm_add_state(&hmm, imm_super(N1));
    imm_hmm_add_state(&hmm, imm_super(M));
    imm_hmm_add_state(&hmm, imm_super(N0));
    imm_hmm_set_start(&hmm, imm_super(N0), 0);

    imm_hmm_set_trans(&hmm, imm_super(N0), imm_super(N1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(N0), imm_super(M), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(N1), imm_super(N2), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(M), imm_super(N2), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(N0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 1, imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 2, 2 * imm_log(0.5));

    DP_RESET(N2, IMM_SUCCESS);
    VITERBI_CHECK(&AB_ab, IMM_SUCCESS, 3, 4 * imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 2, 2 * imm_log(0.5));

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(N0);
    imm_del(M);
    imm_del(N1);
    imm_del(N2);
}

void test_viterbi_global_profile(void)
{
    struct imm_abc abc_z;
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
    AA_z = imm_seq(IMM_STR("AA"), &abc_z);
    AAB_z = imm_seq(IMM_STR("AAB"), &abc_z);
    C_z = imm_seq(IMM_STR("C"), &abc_z);
    CC_z = imm_seq(IMM_STR("CC"), &abc_z);
    CCC_z = imm_seq(IMM_STR("CCC"), &abc_z);
    CCA_z = imm_seq(IMM_STR("CCA"), &abc_z);
    CCAB_z = imm_seq(IMM_STR("CCAB"), &abc_z);
    CCABB_z = imm_seq(IMM_STR("CCABB"), &abc_z);
    CCABA_z = imm_seq(IMM_STR("CCABA"), &abc_z);

    struct imm_hmm hmm = imm_hmm_init(&abc_z);
    struct imm_result result = imm_result();

    struct imm_mute_state *start = imm_mute_state_new(0, &abc_z);

    imm_float B_lprobs[] = {imm_log(0.01), imm_log(0.01), imm_log(1.0), zero()};
    struct imm_normal_state *B = imm_normal_state_new(1, &abc_z, B_lprobs);

    imm_float M0_lprobs[] = {imm_log(0.9), imm_log(0.01), imm_log(0.01),
                             zero()};

    struct imm_normal_state *M0 = imm_normal_state_new(2, &abc_z, M0_lprobs);

    imm_float M1_lprobs[] = {imm_log(0.01), imm_log(0.9), zero(), zero()};
    struct imm_normal_state *M1 = imm_normal_state_new(3, &abc_z, M1_lprobs);

    imm_float M2_lprobs[] = {imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state *M2 = imm_normal_state_new(4, &abc_z, M2_lprobs);

    struct imm_mute_state *E = imm_mute_state_new(5, &abc_z);
    struct imm_mute_state *end = imm_mute_state_new(6, &abc_z);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), imm_log(1.0)};
    struct imm_normal_state *Z = imm_normal_state_new(7, &abc_z, Z_lprobs);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), zero()};
    struct imm_normal_state *I0 = imm_normal_state_new(8, &abc_z, ins_lprobs);
    struct imm_normal_state *I1 = imm_normal_state_new(9, &abc_z, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(10, &abc_z);
    struct imm_mute_state *D2 = imm_mute_state_new(11, &abc_z);

    imm_hmm_add_state(&hmm, imm_super(start));
    imm_hmm_set_start(&hmm, imm_super(start), imm_log(1.0));
    imm_hmm_add_state(&hmm, imm_super(B));
    imm_hmm_add_state(&hmm, imm_super(M0));
    imm_hmm_add_state(&hmm, imm_super(M1));
    imm_hmm_add_state(&hmm, imm_super(M2));
    imm_hmm_add_state(&hmm, imm_super(D1));
    imm_hmm_add_state(&hmm, imm_super(D2));
    imm_hmm_add_state(&hmm, imm_super(I0));
    imm_hmm_add_state(&hmm, imm_super(I1));
    imm_hmm_add_state(&hmm, imm_super(E));
    imm_hmm_add_state(&hmm, imm_super(Z));
    imm_hmm_add_state(&hmm, imm_super(end));

    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(B), 0);
    imm_hmm_set_trans(&hmm, imm_super(B), imm_super(B), 0);
    imm_hmm_set_trans(&hmm, imm_super(B), imm_super(M0), 0);
    imm_hmm_set_trans(&hmm, imm_super(B), imm_super(M1), 0);
    imm_hmm_set_trans(&hmm, imm_super(B), imm_super(M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(M1), 0);
    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(M2), imm_super(E), 0);
    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(E), 0);
    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(E), 0);

    imm_hmm_set_trans(&hmm, imm_super(E), imm_super(end), 0);

    imm_hmm_set_trans(&hmm, imm_super(E), imm_super(Z), 0);
    imm_hmm_set_trans(&hmm, imm_super(Z), imm_super(Z), 0);
    imm_hmm_set_trans(&hmm, imm_super(Z), imm_super(B), 0);

    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(D1), 0);
    imm_hmm_set_trans(&hmm, imm_super(D1), imm_super(D2), 0);
    imm_hmm_set_trans(&hmm, imm_super(D1), imm_super(M2), 0);
    imm_hmm_set_trans(&hmm, imm_super(D2), imm_super(E), 0);

    imm_hmm_set_trans(&hmm, imm_super(M0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I0), imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(&hmm, imm_super(M1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(&hmm, imm_super(I1), imm_super(M2), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(start));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&C_z, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(B, IMM_SUCCESS);
    VITERBI_CHECK(&C_z, IMM_SUCCESS, 2, imm_log(1.0));

    VITERBI_CHECK(&CC_z, IMM_SUCCESS, 3, imm_log(1.0));
    VITERBI_CHECK(&CCC_z, IMM_SUCCESS, 4, imm_log(1.0));
    VITERBI_CHECK(&CCA_z, IMM_SUCCESS, 4, imm_log(0.01));

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(&CCA_z, IMM_SUCCESS, 4, imm_log(0.9));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(&CCAB_z, IMM_SUCCESS, 5, 2 * imm_log(0.9));

    DP_RESET(I0, IMM_SUCCESS);
    VITERBI_CHECK(&CCAB_z, IMM_SUCCESS, 5, imm_log(0.9 * 0.5 * 0.1));

    VITERBI_CHECK(&CCABB_z, IMM_SUCCESS, 6, imm_log(0.9) + 2 * (imm_log(0.05)));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(&CCABA_z, IMM_SUCCESS, 6,
                  imm_log(0.9) + imm_log(0.5) + imm_log(0.1) + imm_log(0.5) +
                      imm_log(0.01));

    DP_RESET(D1, IMM_SUCCESS);
    VITERBI_CHECK(&AA_z, IMM_SUCCESS, 4, imm_log(0.01) + imm_log(0.9));

    DP_RESET(D2, IMM_SUCCESS);
    VITERBI_CHECK(&AA_z, IMM_SUCCESS, 5, imm_log(0.01) + imm_log(0.9));

    DP_RESET(E, IMM_SUCCESS);
    VITERBI_CHECK(&AA_z, IMM_SUCCESS, 6, imm_log(0.01) + imm_log(0.9));

    DP_RESET(M2, IMM_SUCCESS);
    VITERBI_CHECK(&AAB_z, IMM_SUCCESS, 5,
                  imm_log(0.01) + imm_log(0.9) + imm_log(0.5));

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(start);
    imm_del(B);
    imm_del(M0);
    imm_del(M1);
    imm_del(M2);
    imm_del(I0);
    imm_del(I1);
    imm_del(D1);
    imm_del(D2);
    imm_del(Z);
    imm_del(E);
    imm_del(end);
}

void test_viterbi_cycle_mute_ending(void)
{
    struct imm_hmm hmm = imm_hmm_init(&abc_ab);
    struct imm_result result = imm_result();

    struct imm_mute_state *start = imm_mute_state_new(0, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(start));
    imm_hmm_set_start(&hmm, imm_super(start), imm_log(1.0));

    struct imm_mute_state *B = imm_mute_state_new(1, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(B));

    imm_float lprobs[] = {imm_log(0.01), imm_log(0.01)};
    struct imm_normal_state *M = imm_normal_state_new(2, &abc_ab, lprobs);
    imm_hmm_add_state(&hmm, imm_super(M));

    struct imm_mute_state *E = imm_mute_state_new(3, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(E));

    struct imm_mute_state *J = imm_mute_state_new(4, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(J));

    struct imm_mute_state *end = imm_mute_state_new(5, &abc_ab);
    imm_hmm_add_state(&hmm, imm_super(end));

    imm_hmm_set_trans(&hmm, imm_super(start), imm_super(B), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(B), imm_super(M), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(M), imm_super(E), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(E), imm_super(end), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(E), imm_super(J), imm_log(0.1));
    imm_hmm_set_trans(&hmm, imm_super(J), imm_super(B), imm_log(0.1));

    struct imm_dp *dp = imm_hmm_new_dp(&hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(&A_ab, IMM_SUCCESS, 5, -13.815510557964272);

    unsigned BM = imm_dp_trans_idx(dp, imm_state_idx(imm_super(B)),
                                   imm_state_idx(imm_super(M)));

    imm_dp_change_trans(dp, BM, imm_log(1.0));

    EQ(imm_task_setup(task, &A_ab), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 5);
    CLOSE(result.loglik, -11.5129254650);

    imm_del(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(start);
    imm_del(B);
    imm_del(M);
    imm_del(E);
    imm_del(J);
    imm_del(end);
}
