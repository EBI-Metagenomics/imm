#include "hope/hope.h"
#include "imm/error.h"
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

int main(void)
{
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
    CLOSE(imm_hmm_loglik(hmm, seq, &result.path), (llik));                     \
    CLOSE(result.loglik, (llik));

#define DP_RESET(end_state, status)                                            \
    imm_del(dp);                                                               \
    dp = imm_hmm_new_dp(hmm, imm_super(end_state));                            \
    EQ(imm_task_reset(task, dp), status);

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int is_valid(imm_float a) { return imm_lprob_is_valid(a); }

void test_viterbi_one_mute_state(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_seq const *C = imm_seq_new(IMM_STR("C"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *state = imm_mute_state_new(0, abc);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(EMPTY, IMM_SUCCESS, 1, imm_log(0.5));
    VITERBI_CHECK(C, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_deinit(&result);
    imm_del(dp);
    imm_del(task);
    imm_del(hmm);
    imm_del(state);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(C);
}

void test_viterbi_two_mute_states(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *state0 = imm_mute_state_new(0, abc);
    struct imm_mute_state *state1 = imm_mute_state_new(1, abc);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.5));
    imm_hmm_add_state(hmm, imm_super(state1));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(EMPTY, IMM_SUCCESS, 1, imm_log(0.5));

    EQ(imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero()),
       IMM_ILLEGALARG);

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.1));

    DP_RESET(state1, IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    EQ(imm_path_step(&result.path, 0)->state_id, 0);
    EQ(imm_path_step(&result.path, 0)->seqlen, 0);
    EQ(imm_path_step(&result.path, 1)->state_id, 1);
    EQ(imm_path_step(&result.path, 1)->seqlen, 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path),
          imm_log(0.5) + imm_log(0.1));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.1));

    imm_deinit(&result);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
    imm_del(task);
    imm_del(dp);
    imm_del(EMPTY);
}

void test_viterbi_mute_cycle(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *state0 = imm_mute_state_new(0, abc);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.5));

    struct imm_mute_state *state1 = imm_mute_state_new(1, abc);
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state0), imm_log(0.2));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    ISNULL(dp);

    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
    imm_del(EMPTY);
}

void test_viterbi_one_normal_state(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *T = imm_seq_new(IMM_STR("T"), abc);
    struct imm_seq const *AA = imm_seq_new(IMM_STR("AA"), abc);
    struct imm_seq const *AC = imm_seq_new(IMM_STR("AC"), abc);
    struct imm_seq const *ACT = imm_seq_new(IMM_STR("ACT"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state = imm_normal_state_new(0, abc, lprobs0);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(1.0) + imm_log(0.25));
    VITERBI_CHECK(T, IMM_SUCCESS, 0, imm_lprob_nan());
    VITERBI_CHECK(AC, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.1));
    DP_RESET(state, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(1.0) + imm_log(0.25));
    VITERBI_CHECK(AA, IMM_SUCCESS, 2, imm_log(0.1) + 2 * imm_log(0.25));
    VITERBI_CHECK(ACT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_normalize_trans(hmm);
    DP_RESET(state, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(AA, IMM_SUCCESS, 2, 2 * imm_log(0.25));
    VITERBI_CHECK(ACT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(state);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(T);
    imm_del(AA);
    imm_del(AC);
    imm_del(ACT);
}

void test_viterbi_two_normal_states(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *T = imm_seq_new(IMM_STR("T"), abc);
    struct imm_seq const *AC = imm_seq_new(IMM_STR("AC"), abc);
    struct imm_seq const *AT = imm_seq_new(IMM_STR("AT"), abc);
    struct imm_seq const *ATT = imm_seq_new(IMM_STR("ATT"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_log(0.5)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.1));
    imm_hmm_add_state(hmm, imm_super(state1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.3));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.1) + imm_log(0.25));
    VITERBI_CHECK(T, IMM_SUCCESS, 0, imm_lprob_nan());
    VITERBI_CHECK(AC, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(AT, IMM_SUCCESS, 2,
                  imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));

    VITERBI_CHECK(ATT, IMM_SUCCESS, 0, imm_lprob_nan());

    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(0.5));
    imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(ATT, IMM_SUCCESS, 3,
                  imm_log(0.1) + imm_log(0.25) + imm_log(0.3) +
                      3 * imm_log(0.5));

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(T);
    imm_del(AC);
    imm_del(AT);
    imm_del(ATT);
}

void test_viterbi_normal_states(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ACGT"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *AA = imm_seq_new(IMM_STR("AA"), abc);
    struct imm_seq const *AG = imm_seq_new(IMM_STR("AG"), abc);
    struct imm_seq const *AGT = imm_seq_new(IMM_STR("AGT"), abc);
    struct imm_seq const *AGTC = imm_seq_new(IMM_STR("AGTC"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    imm_float const lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                                 zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, abc, lprobs0);

    imm_float const lprobs1[] = {
        imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),
        imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(1.0));

    imm_hmm_normalize_trans(hmm);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, -1.386294361120);
    VITERBI_CHECK(AG, IMM_SUCCESS, 2, -3.178053830348);

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(AG, IMM_SUCCESS, 2, -3.295836866004);

    DP_RESET(state0, IMM_SUCCESS);
    VITERBI_CHECK(AGT, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(AGT, IMM_SUCCESS, 3, -4.106767082221);

    DP_RESET(state0, IMM_SUCCESS);
    VITERBI_CHECK(AGTC, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(state1, IMM_SUCCESS);
    VITERBI_CHECK(AGTC, IMM_SUCCESS, 4, -6.303991659557);

    EQ(imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state0), zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), zero()),
       IMM_ILLEGALARG);

    EQ(imm_hmm_set_start(hmm, imm_super(state0), zero()), IMM_ILLEGALARG);
    EQ(imm_hmm_set_start(hmm, imm_super(state1), zero()), IMM_ILLEGALARG);

    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));

    DP_RESET(state0, IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    DP_RESET(state1, IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.25));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.9));

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(AA, IMM_SUCCESS, 2, 2 * imm_log(0.25) + imm_log(0.9));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));

    DP_RESET(state0, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.25));
    VITERBI_CHECK(AA, IMM_SUCCESS, 2, 2 * imm_log(0.25) + imm_log(0.9));

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(AA);
    imm_del(AG);
    imm_del(AGT);
    imm_del(AGTC);
}

void test_viterbi_profile1(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("AB"), '*');
    struct imm_seq const *EMPTY = imm_seq_new(IMM_STR(""), abc);
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *B = imm_seq_new(IMM_STR("B"), abc);
    struct imm_seq const *AA = imm_seq_new(IMM_STR("AA"), abc);
    struct imm_seq const *AAB = imm_seq_new(IMM_STR("AAB"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *start = imm_mute_state_new(0, abc);
    struct imm_mute_state *D0 = imm_mute_state_new(1, abc);
    struct imm_mute_state *end = imm_mute_state_new(2, abc);

    imm_float M0_lprobs[] = {imm_log(0.4), imm_log(0.2)};
    struct imm_normal_state *M0 = imm_normal_state_new(3, abc, M0_lprobs);

    imm_float I0_lprobs[] = {imm_log(0.5), imm_log(0.5)};
    struct imm_normal_state *I0 = imm_normal_state_new(4, abc, I0_lprobs);

    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(D0));
    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_add_state(hmm, imm_super(M0));
    imm_hmm_add_state(hmm, imm_super(I0));

    imm_hmm_set_trans(hmm, imm_super(start), imm_super(D0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(D0), imm_super(end), imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_super(start), imm_super(M0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(end), imm_log(0.8));
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(I0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(end), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(EMPTY, IMM_SUCCESS, 3, imm_log(0.1) + imm_log(1.0));

    DP_RESET(D0, IMM_SUCCESS);
    VITERBI_CHECK(EMPTY, IMM_SUCCESS, 2, imm_log(0.1));

    DP_RESET(start, IMM_SUCCESS);
    VITERBI_CHECK(EMPTY, IMM_SUCCESS, 1, imm_log(1.0));

    DP_RESET(M0, IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, 2, imm_log(0.5) + imm_log(0.4));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, 3,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.8));

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(B, IMM_SUCCESS, 2, imm_log(0.5) + imm_log(0.2));

    DP_RESET(end, IMM_SUCCESS);
    VITERBI_CHECK(B, IMM_SUCCESS, 3,
                  imm_log(0.5) + imm_log(0.2) + imm_log(0.8));
    VITERBI_CHECK(AA, IMM_SUCCESS, 4,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5));

    VITERBI_CHECK(AAB, IMM_SUCCESS, 5,
                  imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) +
                      2 * imm_log(0.5));

    imm_deinit(&result);
    imm_del(dp);
    imm_del(task);
    imm_del(hmm);
    imm_del(start);
    imm_del(D0);
    imm_del(M0);
    imm_del(I0);
    imm_del(end);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(B);
    imm_del(AA);
    imm_del(AAB);
}

void test_viterbi_profile2(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ABCD"), '*');
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *B = imm_seq_new(IMM_STR("B"), abc);
    struct imm_seq const *C = imm_seq_new(IMM_STR("C"), abc);
    struct imm_seq const *D = imm_seq_new(IMM_STR("D"), abc);
    struct imm_seq const *CA = imm_seq_new(IMM_STR("CA"), abc);
    struct imm_seq const *CD = imm_seq_new(IMM_STR("CD"), abc);
    struct imm_seq const *CDDDA = imm_seq_new(IMM_STR("CDDDA"), abc);
    struct imm_seq const *CDDDAB = imm_seq_new(IMM_STR("CDDDAB"), abc);
    struct imm_seq const *CDDDABA = imm_seq_new(IMM_STR("CDDDABA"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *start = imm_mute_state_new(0, abc);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1),
                              imm_log(0.7)};

    imm_float M0_lprobs[] = {imm_log(0.4), zero(), imm_log(0.6), zero()};
    imm_float M1_lprobs[] = {imm_log(0.6), zero(), imm_log(0.4), zero()};
    imm_float M2_lprobs[] = {imm_log(0.05), imm_log(0.05), imm_log(0.05),
                             imm_log(0.05)};

    struct imm_normal_state *M0 = imm_normal_state_new(1, abc, M0_lprobs);
    struct imm_normal_state *I0 = imm_normal_state_new(2, abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(3, abc);
    struct imm_normal_state *M1 = imm_normal_state_new(4, abc, M1_lprobs);
    struct imm_normal_state *I1 = imm_normal_state_new(5, abc, ins_lprobs);

    struct imm_mute_state *D2 = imm_mute_state_new(6, abc);
    struct imm_normal_state *M2 = imm_normal_state_new(7, abc, M2_lprobs);

    struct imm_mute_state *end = imm_mute_state_new(8, abc);

    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), 0.0);

    imm_hmm_add_state(hmm, imm_super(M0));
    imm_hmm_add_state(hmm, imm_super(I0));

    imm_hmm_add_state(hmm, imm_super(D1));
    imm_hmm_add_state(hmm, imm_super(M1));
    imm_hmm_add_state(hmm, imm_super(I1));

    imm_hmm_add_state(hmm, imm_super(D2));
    imm_hmm_add_state(hmm, imm_super(M2));

    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_set_trans(hmm, imm_super(start), imm_super(M0), 0.0);
    imm_hmm_set_trans(hmm, imm_super(start), imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(start), imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M2), imm_super(end), 0.0);

    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(D1), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(D2), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(I1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(I1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1), imm_super(M2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(D2), imm_log(0.3));
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(M2), imm_log(0.7));

    imm_hmm_set_trans(hmm, imm_super(D2), imm_super(end), imm_log(1.0));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(M2));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    DP_RESET(M2, IMM_SUCCESS);

    VITERBI_CHECK(B, IMM_SUCCESS, 2, imm_log(0.05));

    VITERBI_CHECK(C, IMM_SUCCESS, 2, imm_log(0.05));

    VITERBI_CHECK(D, IMM_SUCCESS, 2, imm_log(0.05));

    DP_RESET(end, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 3, imm_log(0.6));

    VITERBI_CHECK(B, IMM_SUCCESS, 3, imm_log(0.05));

    VITERBI_CHECK(C, IMM_SUCCESS, 3, imm_log(0.6));

    VITERBI_CHECK(D, IMM_SUCCESS, 3, imm_log(0.05));

    DP_RESET(M1, IMM_SUCCESS);

    VITERBI_CHECK(A, IMM_SUCCESS, 2, imm_log(0.6));

    VITERBI_CHECK(C, IMM_SUCCESS, 2, imm_log(0.4));

    DP_RESET(end, IMM_SUCCESS);

    VITERBI_CHECK(CA, IMM_SUCCESS, 4, 2 * imm_log(0.6));

    DP_RESET(I0, IMM_SUCCESS);

    VITERBI_CHECK(CD, IMM_SUCCESS, 3,
                  imm_log(0.6) + imm_log(0.2) + imm_log(0.7));

    DP_RESET(end, IMM_SUCCESS);

    VITERBI_CHECK(CDDDA, IMM_SUCCESS, 7,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6));

    VITERBI_CHECK(CDDDAB, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.05));

    DP_RESET(M2, IMM_SUCCESS);

    VITERBI_CHECK(CDDDABA, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                      3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.2) +
                      imm_log(0.1) + imm_log(0.5) + imm_log(0.05));

    DP_RESET(M1, IMM_SUCCESS);

    VITERBI_CHECK(CDDDABA, IMM_SUCCESS, 8,
                  imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
                      3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6));

    DP_RESET(end, IMM_SUCCESS);

    VITERBI_CHECK(CDDDABA, IMM_SUCCESS, 9,
                  imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
                      3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6));

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(start);
    imm_del(M0);
    imm_del(I0);
    imm_del(D1);
    imm_del(M1);
    imm_del(I1);
    imm_del(D2);
    imm_del(M2);
    imm_del(end);
    imm_del(abc);
    imm_del(A);
    imm_del(B);
    imm_del(C);
    imm_del(D);
    imm_del(CA);
    imm_del(CD);
    imm_del(CDDDA);
    imm_del(CDDDAB);
    imm_del(CDDDABA);
}

void test_viterbi_profile_delete(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("AB"), '*');
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *AB = imm_seq_new(IMM_STR("AB"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    imm_float N0_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N0 = imm_normal_state_new(0, abc, N0_lprobs);

    struct imm_mute_state *M = imm_mute_state_new(1, abc);

    imm_float N1_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N1 = imm_normal_state_new(2, abc, N1_lprobs);

    imm_float N2_lprobs[] = {zero(), imm_log(0.5)};
    struct imm_normal_state *N2 = imm_normal_state_new(3, abc, N2_lprobs);

    imm_hmm_add_state(hmm, imm_super(N2));
    imm_hmm_add_state(hmm, imm_super(N1));
    imm_hmm_add_state(hmm, imm_super(M));
    imm_hmm_add_state(hmm, imm_super(N0));
    imm_hmm_set_start(hmm, imm_super(N0), 0);

    imm_hmm_set_trans(hmm, imm_super(N0), imm_super(N1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(N0), imm_super(M), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(N1), imm_super(N2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(M), imm_super(N2), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(N0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(A, IMM_SUCCESS, 1, imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, 2, 2 * imm_log(0.5));

    DP_RESET(N2, IMM_SUCCESS);
    VITERBI_CHECK(AB, IMM_SUCCESS, 3, 4 * imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, 2, 2 * imm_log(0.5));

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(N0);
    imm_del(M);
    imm_del(N1);
    imm_del(N2);
    imm_del(abc);
    imm_del(A);
    imm_del(AB);
}

void test_viterbi_global_profile(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("ABCZ"), '*');
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_seq const *AA = imm_seq_new(IMM_STR("AA"), abc);
    struct imm_seq const *AAB = imm_seq_new(IMM_STR("AAB"), abc);
    struct imm_seq const *C = imm_seq_new(IMM_STR("C"), abc);
    struct imm_seq const *CC = imm_seq_new(IMM_STR("CC"), abc);
    struct imm_seq const *CCC = imm_seq_new(IMM_STR("CCC"), abc);
    struct imm_seq const *CCA = imm_seq_new(IMM_STR("CCA"), abc);
    struct imm_seq const *CCAB = imm_seq_new(IMM_STR("CCAB"), abc);
    struct imm_seq const *CCABB = imm_seq_new(IMM_STR("CCABB"), abc);
    struct imm_seq const *CCABA = imm_seq_new(IMM_STR("CCABA"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *start = imm_mute_state_new(0, abc);

    imm_float B_lprobs[] = {imm_log(0.01), imm_log(0.01), imm_log(1.0), zero()};
    struct imm_normal_state *B = imm_normal_state_new(1, abc, B_lprobs);

    imm_float M0_lprobs[] = {imm_log(0.9), imm_log(0.01), imm_log(0.01),
                             zero()};

    struct imm_normal_state *M0 = imm_normal_state_new(2, abc, M0_lprobs);

    imm_float M1_lprobs[] = {imm_log(0.01), imm_log(0.9), zero(), zero()};
    struct imm_normal_state *M1 = imm_normal_state_new(3, abc, M1_lprobs);

    imm_float M2_lprobs[] = {imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state *M2 = imm_normal_state_new(4, abc, M2_lprobs);

    struct imm_mute_state *E = imm_mute_state_new(5, abc);
    struct imm_mute_state *end = imm_mute_state_new(6, abc);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), imm_log(1.0)};
    struct imm_normal_state *Z = imm_normal_state_new(7, abc, Z_lprobs);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), zero()};
    struct imm_normal_state *I0 = imm_normal_state_new(8, abc, ins_lprobs);
    struct imm_normal_state *I1 = imm_normal_state_new(9, abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(10, abc);
    struct imm_mute_state *D2 = imm_mute_state_new(11, abc);

    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(B));
    imm_hmm_add_state(hmm, imm_super(M0));
    imm_hmm_add_state(hmm, imm_super(M1));
    imm_hmm_add_state(hmm, imm_super(M2));
    imm_hmm_add_state(hmm, imm_super(D1));
    imm_hmm_add_state(hmm, imm_super(D2));
    imm_hmm_add_state(hmm, imm_super(I0));
    imm_hmm_add_state(hmm, imm_super(I1));
    imm_hmm_add_state(hmm, imm_super(E));
    imm_hmm_add_state(hmm, imm_super(Z));
    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_set_trans(hmm, imm_super(start), imm_super(B), 0);
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(B), 0);
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(M0), 0);
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_super(M2), imm_super(E), 0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(E), 0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(E), 0);

    imm_hmm_set_trans(hmm, imm_super(E), imm_super(end), 0);

    imm_hmm_set_trans(hmm, imm_super(E), imm_super(Z), 0);
    imm_hmm_set_trans(hmm, imm_super(Z), imm_super(Z), 0);
    imm_hmm_set_trans(hmm, imm_super(Z), imm_super(B), 0);

    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(D1), 0);
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(D2), 0);
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_super(D2), imm_super(E), 0);

    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0), imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1), imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1), imm_super(M2), imm_log(0.5));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(start));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(C, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(B, IMM_SUCCESS);
    VITERBI_CHECK(C, IMM_SUCCESS, 2, imm_log(1.0));

    VITERBI_CHECK(CC, IMM_SUCCESS, 3, imm_log(1.0));

    VITERBI_CHECK(CCC, IMM_SUCCESS, 4, imm_log(1.0));

    VITERBI_CHECK(CCA, IMM_SUCCESS, 4, imm_log(0.01));

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(CCA, IMM_SUCCESS, 4, imm_log(0.9));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(CCAB, IMM_SUCCESS, 5, 2 * imm_log(0.9));

    DP_RESET(I0, IMM_SUCCESS);
    VITERBI_CHECK(CCAB, IMM_SUCCESS, 5, imm_log(0.9 * 0.5 * 0.1));

    VITERBI_CHECK(CCABB, IMM_SUCCESS, 6, imm_log(0.9) + 2 * (imm_log(0.05)));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(CCABA, IMM_SUCCESS, 6,
                  imm_log(0.9) + imm_log(0.5) + imm_log(0.1) + imm_log(0.5) +
                      imm_log(0.01));

    DP_RESET(D1, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, 4, imm_log(0.01) + imm_log(0.9));

    DP_RESET(D2, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, 5, imm_log(0.01) + imm_log(0.9));

    DP_RESET(E, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, 6, imm_log(0.01) + imm_log(0.9));

    DP_RESET(M2, IMM_SUCCESS);
    VITERBI_CHECK(AAB, IMM_SUCCESS, 5,
                  imm_log(0.01) + imm_log(0.9) + imm_log(0.5));

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
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
    imm_del(abc);
    imm_del(A);
    imm_del(AA);
    imm_del(AAB);
    imm_del(C);
    imm_del(CC);
    imm_del(CCC);
    imm_del(CCA);
    imm_del(CCAB);
    imm_del(CCABB);
    imm_del(CCABA);
}

void test_viterbi_cycle_mute_ending(void)
{
    struct imm_abc const *abc = imm_abc_new(IMM_STR("AB"), '*');
    struct imm_seq const *A = imm_seq_new(IMM_STR("A"), abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);
    struct imm_result result = IMM_RESULT_INIT();

    struct imm_mute_state *start = imm_mute_state_new(0, abc);
    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), imm_log(1.0));

    struct imm_mute_state *B = imm_mute_state_new(1, abc);
    imm_hmm_add_state(hmm, imm_super(B));

    imm_float lprobs[] = {imm_log(0.01), imm_log(0.01)};
    struct imm_normal_state *M = imm_normal_state_new(2, abc, lprobs);
    imm_hmm_add_state(hmm, imm_super(M));

    struct imm_mute_state *E = imm_mute_state_new(3, abc);
    imm_hmm_add_state(hmm, imm_super(E));

    struct imm_mute_state *J = imm_mute_state_new(4, abc);
    imm_hmm_add_state(hmm, imm_super(J));

    struct imm_mute_state *end = imm_mute_state_new(5, abc);
    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_set_trans(hmm, imm_super(start), imm_super(B), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(M), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(M), imm_super(E), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(end), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(J), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(J), imm_super(B), imm_log(0.1));

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(A, IMM_SUCCESS, 5, -13.815510557964272);

    unsigned BM = imm_dp_trans_idx(dp, imm_state_id(imm_super(B)),
                                   imm_state_id(imm_super(M)));

    imm_dp_change_trans(dp, BM, imm_log(1.0));

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 5);
    CLOSE(result.loglik, -11.5129261017);

    imm_deinit(&result);
    imm_del(task);
    imm_del(dp);
    imm_del(hmm);
    imm_del(start);
    imm_del(B);
    imm_del(M);
    imm_del(E);
    imm_del(J);
    imm_del(end);
    imm_del(abc);
    imm_del(A);
}
