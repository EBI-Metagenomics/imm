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

/* TODO: temporary, remove it by something else */
#define VITERBI_CHECK(seq, e0, e1, nsteps, llik)                               \
    EQ(imm_task_setup(task, seq), e0);                                         \
    EQ(imm_dp_viterbi(dp, task, &result), e1);                                 \
    EQ(imm_path_nsteps(&result.path), nsteps);                                 \
    CLOSE(imm_hmm_loglik(hmm, seq, &result.path), (llik));                     \
    CLOSE(result.loglik, (llik));

#define DP_RESET(end_state, e0)                                                \
    imm_del(dp);                                                               \
    dp = imm_hmm_new_dp(hmm, imm_super(end_state));                            \
    EQ(imm_task_reset(task, dp), e0);

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int is_valid(imm_float a) { return imm_lprob_is_valid(a); }

void test_viterbi_one_mute_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *C = imm_seq_new(1, "C", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *state = imm_mute_state_new(0, abc);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(0.5));

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(0.5));
    CLOSE(result.loglik, imm_log(0.5));

    EQ(imm_task_setup(task, C), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, C, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

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
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *state0 = imm_mute_state_new(0, abc);
    struct imm_mute_state *state1 = imm_mute_state_new(1, abc);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.5));
    imm_hmm_add_state(hmm, imm_super(state1));

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(0.5));
    CLOSE(result.loglik, imm_log(0.5));

    EQ(imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero()),
       IMM_ILLEGALARG);

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.1));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

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
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(1, "", abc);
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
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *T = imm_seq_new(1, "T", abc);
    struct imm_seq const *AA = imm_seq_new(2, "AA", abc);
    struct imm_seq const *AC = imm_seq_new(2, "AC", abc);
    struct imm_seq const *ACT = imm_seq_new(3, "ACT", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state = imm_normal_state_new(0, abc, lprobs0);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(1.0) + imm_log(0.25));
    CLOSE(result.loglik, imm_log(1.0) + imm_log(0.25));

    EQ(imm_task_setup(task, T), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, T, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    EQ(imm_task_setup(task, AC), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, AC, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.1));
    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(1.0) + imm_log(0.25));
    CLOSE(result.loglik, imm_log(1.0) + imm_log(0.25));

    EQ(imm_task_setup(task, AA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AA, &result.path),
          imm_log(0.1) + 2 * imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.1) + 2 * imm_log(0.25));

    EQ(imm_task_setup(task, ACT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, ACT, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_hmm_normalize_trans(hmm);
    imm_del(dp);
    imm_del(task);
    dp = imm_hmm_new_dp(hmm, imm_super(state));
    task = imm_task_new(dp);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.25));

    EQ(imm_task_setup(task, AA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AA, &result.path), 2 * imm_log(0.25));
    CLOSE(result.loglik, 2 * imm_log(0.25));

    EQ(imm_task_setup(task, ACT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, ACT, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

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
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *T = imm_seq_new(1, "T", abc);
    struct imm_seq const *AC = imm_seq_new(2, "AC", abc);
    struct imm_seq const *AT = imm_seq_new(2, "AT", abc);
    struct imm_seq const *ATT = imm_seq_new(3, "ATT", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_log(0.5)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(0.1));
    imm_hmm_add_state(hmm, imm_super(state1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.3));

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.1) + imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.1) + imm_log(0.25));

    EQ(imm_task_setup(task, T), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, T, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    EQ(imm_task_setup(task, AC), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, AC, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AT, &result.path),
          imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));
    CLOSE(result.loglik,
          imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));

    EQ(imm_task_setup(task, ATT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, ATT, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(0.5));
    imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, ATT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, ATT, &result.path),
          imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + 3 * imm_log(0.5));
    CLOSE(result.loglik,
          imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + 3 * imm_log(0.5));

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
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *AA = imm_seq_new(2, "AA", abc);
    struct imm_seq const *AG = imm_seq_new(2, "AG", abc);
    struct imm_seq const *AGT = imm_seq_new(3, "AGT", abc);
    struct imm_seq const *AGTC = imm_seq_new(4, "AGTC", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), -1.386294361120);
    CLOSE(result.loglik, -1.386294361120);

    EQ(imm_task_setup(task, AG), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AG, &result.path), -3.178053830348);
    CLOSE(result.loglik, -3.178053830348);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AG), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AG, &result.path), -3.295836866004);
    CLOSE(result.loglik, -3.295836866004);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AGT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, AGT, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AGT), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, AGT, &result.path), -4.106767082221);
    CLOSE(result.loglik, -4.106767082221);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AGTC), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, AGTC, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, AGTC), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 4);
    CLOSE(imm_hmm_loglik(hmm, AGTC, &result.path), -6.303991659557);
    CLOSE(result.loglik, -6.303991659557);

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

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.25));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.9));
    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.25));

    EQ(imm_task_setup(task, AA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AA, &result.path),
          2 * imm_log(0.25) + imm_log(0.9));
    CLOSE(result.loglik, 2 * imm_log(0.25) + imm_log(0.9));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.25));
    CLOSE(result.loglik, imm_log(0.25));

    EQ(imm_task_setup(task, AA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, AA, &result.path),
          2 * imm_log(0.25) + imm_log(0.9));
    CLOSE(result.loglik, 2 * imm_log(0.25) + imm_log(0.9));

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
    struct imm_abc const *abc = imm_abc_new(2, "AB", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *B = imm_seq_new(1, "B", abc);
    struct imm_seq const *AA = imm_seq_new(2, "AA", abc);
    struct imm_seq const *AAB = imm_seq_new(3, "AAB", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path),
          imm_log(0.1) + imm_log(1.0));
    CLOSE(result.loglik, imm_log(0.1) + imm_log(1.0));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(D0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(0.1));
    CLOSE(result.loglik, imm_log(0.1));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(start));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 1);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(1.0));
    CLOSE(result.loglik, imm_log(1.0));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    EQ(imm_path_nsteps(&result.path), 0);
    CLOSE(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_nan());
    CLOSE(result.loglik, imm_lprob_nan());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.5) + imm_log(0.4));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.4));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path),
          imm_log(0.5) + imm_log(0.4) + imm_log(0.8));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.4) + imm_log(0.8));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, B), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, B, &result.path), imm_log(0.5) + imm_log(0.2));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.2));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, B), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, B, &result.path),
          imm_log(0.5) + imm_log(0.2) + imm_log(0.8));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.2) + imm_log(0.8));

    EQ(imm_task_setup(task, AA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 4);
    CLOSE(imm_hmm_loglik(hmm, AA, &result.path),
          imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5));
    CLOSE(result.loglik,
          imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5));

    EQ(imm_task_setup(task, AAB), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 5);
    CLOSE(imm_hmm_loglik(hmm, AAB, &result.path),
          imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) +
              2 * imm_log(0.5));
    CLOSE(result.loglik, imm_log(0.5) + imm_log(0.4) + imm_log(0.1) +
                             imm_log(0.2) + 2 * imm_log(0.5));

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
    struct imm_abc const *abc = imm_abc_new(4, "ABCD", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *B = imm_seq_new(1, "B", abc);
    struct imm_seq const *C = imm_seq_new(1, "C", abc);
    struct imm_seq const *D = imm_seq_new(1, "D", abc);
    struct imm_seq const *CA = imm_seq_new(2, "CA", abc);
    struct imm_seq const *CD = imm_seq_new(2, "CD", abc);
    struct imm_seq const *CDDDA = imm_seq_new(5, "CDDDA", abc);
    struct imm_seq const *CDDDAB = imm_seq_new(6, "CDDDAB", abc);
    struct imm_seq const *CDDDABA = imm_seq_new(7, "CDDDABA", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(M2));
    struct imm_task *task = imm_task_new(dp);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M2));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, B), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, B, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    EQ(imm_task_setup(task, C), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, C, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    EQ(imm_task_setup(task, D), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, D, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.6));
    CLOSE(result.loglik, imm_log(0.6));

    EQ(imm_task_setup(task, B), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, B, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    EQ(imm_task_setup(task, C), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, C, &result.path), imm_log(0.6));
    CLOSE(result.loglik, imm_log(0.6));

    EQ(imm_task_setup(task, D), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, D, &result.path), imm_log(0.05));
    CLOSE(result.loglik, imm_log(0.05));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, A), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.6));
    CLOSE(result.loglik, imm_log(0.6));

    EQ(imm_task_setup(task, C), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 2);
    CLOSE(imm_hmm_loglik(hmm, C, &result.path), imm_log(0.4));
    CLOSE(result.loglik, imm_log(0.4));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, CA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 4);
    CLOSE(imm_hmm_loglik(hmm, CA, &result.path), 2 * imm_log(0.6));
    CLOSE(result.loglik, 2 * imm_log(0.6));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(I0));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, CD), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 3);
    CLOSE(imm_hmm_loglik(hmm, CD, &result.path),
          imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    CLOSE(result.loglik, imm_log(0.6) + imm_log(0.2) + imm_log(0.7));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    EQ(imm_task_setup(task, CDDDA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 7);
    CLOSE(imm_hmm_loglik(hmm, CDDDA, &result.path),
          imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) +
              imm_log(0.6));
    CLOSE(result.loglik, imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                             3 * imm_log(0.5) + imm_log(0.6));

    imm_float desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                        3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.05);
    EQ(imm_task_setup(task, CDDDAB), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 8);
    CLOSE(imm_hmm_loglik(hmm, CDDDAB, &result.path), desired);
    CLOSE(result.loglik, desired);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M2));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
              3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.2) + imm_log(0.1) +
              imm_log(0.5) + imm_log(0.05);
    EQ(imm_task_setup(task, CDDDABA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 8);
    CLOSE(imm_hmm_loglik(hmm, CDDDABA, &result.path), desired);
    CLOSE(result.loglik, desired);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(M1));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
              3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);
    EQ(imm_task_setup(task, CDDDABA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 8);
    CLOSE(imm_hmm_loglik(hmm, CDDDABA, &result.path), desired);
    CLOSE(result.loglik, desired);

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(end));
    EQ(imm_task_reset(task, dp), IMM_SUCCESS);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
              3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);
    EQ(imm_task_setup(task, CDDDABA), IMM_SUCCESS);
    EQ(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    EQ(imm_path_nsteps(&result.path), 9);
    CLOSE(imm_hmm_loglik(hmm, CDDDABA, &result.path), desired);
    CLOSE(result.loglik, desired);

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
    struct imm_abc const *abc = imm_abc_new(2, "AB", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *AB = imm_seq_new(2, "AB", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(N0));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(A, IMM_SUCCESS, IMM_SUCCESS, 1, imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, IMM_SUCCESS, 2, 2 * imm_log(0.5));

    DP_RESET(N2, IMM_SUCCESS);
    VITERBI_CHECK(AB, IMM_SUCCESS, IMM_SUCCESS, 3, 4 * imm_log(0.5));

    DP_RESET(M, IMM_SUCCESS);
    VITERBI_CHECK(A, IMM_SUCCESS, IMM_SUCCESS, 2, 2 * imm_log(0.5));

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
    struct imm_abc const *abc = imm_abc_new(4, "ABCZ", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *AA = imm_seq_new(2, "AA", abc);
    struct imm_seq const *AAB = imm_seq_new(3, "AAB", abc);
    struct imm_seq const *C = imm_seq_new(1, "C", abc);
    struct imm_seq const *CC = imm_seq_new(2, "CC", abc);
    struct imm_seq const *CCC = imm_seq_new(3, "CCC", abc);
    struct imm_seq const *CCA = imm_seq_new(3, "CCA", abc);
    struct imm_seq const *CCAB = imm_seq_new(4, "CCAB", abc);
    struct imm_seq const *CCABB = imm_seq_new(5, "CCABB", abc);
    struct imm_seq const *CCABA = imm_seq_new(5, "CCABA", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(start));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(C, IMM_SUCCESS, IMM_SUCCESS, 0, imm_lprob_nan());

    DP_RESET(B, IMM_SUCCESS);
    VITERBI_CHECK(C, IMM_SUCCESS, IMM_SUCCESS, 2, imm_log(1.0));

    VITERBI_CHECK(CC, IMM_SUCCESS, IMM_SUCCESS, 3, imm_log(1.0));

    VITERBI_CHECK(CCC, IMM_SUCCESS, IMM_SUCCESS, 4, imm_log(1.0));

    VITERBI_CHECK(CCA, IMM_SUCCESS, IMM_SUCCESS, 4, imm_log(0.01));

    DP_RESET(M0, IMM_SUCCESS);
    VITERBI_CHECK(CCA, IMM_SUCCESS, IMM_SUCCESS, 4, imm_log(0.9));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(CCAB, IMM_SUCCESS, IMM_SUCCESS, 5, 2 * imm_log(0.9));

    DP_RESET(I0, IMM_SUCCESS);
    VITERBI_CHECK(CCAB, IMM_SUCCESS, IMM_SUCCESS, 5, imm_log(0.9 * 0.5 * 0.1));

    VITERBI_CHECK(CCABB, IMM_SUCCESS, IMM_SUCCESS, 6,
                  imm_log(0.9) + 2 * (imm_log(0.05)));

    DP_RESET(M1, IMM_SUCCESS);
    VITERBI_CHECK(CCABA, IMM_SUCCESS, IMM_SUCCESS, 6,
                  imm_log(0.9) + imm_log(0.5) + imm_log(0.1) + imm_log(0.5) +
                      imm_log(0.01));

    DP_RESET(D1, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, IMM_SUCCESS, 4,
                  imm_log(0.01) + imm_log(0.9));

    DP_RESET(D2, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, IMM_SUCCESS, 5,
                  imm_log(0.01) + imm_log(0.9));

    DP_RESET(E, IMM_SUCCESS);
    VITERBI_CHECK(AA, IMM_SUCCESS, IMM_SUCCESS, 6,
                  imm_log(0.01) + imm_log(0.9));

    DP_RESET(M2, IMM_SUCCESS);
    VITERBI_CHECK(AAB, IMM_SUCCESS, IMM_SUCCESS, 5,
                  imm_log(0.01) + imm_log(0.9) + imm_log(0.5));

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
    struct imm_abc const *abc = imm_abc_new(2, "AB", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

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

    struct imm_result result = IMM_RESULT_INIT();

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(end));
    struct imm_task *task = imm_task_new(dp);

    VITERBI_CHECK(A, IMM_SUCCESS, IMM_SUCCESS, 5, -13.815510557964272);

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
