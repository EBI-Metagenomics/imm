#include "cass/cass.h"
#include "imm/dp.h"
#include "imm/error.h"
#include "imm/hmm.h"
#include "imm/imm.h"
#include "imm/lprob.h"

void test_viterbi_one_mute_state(void);
void test_viterbi_two_mute_states(void);
void test_viterbi_mute_cycle(void);
void test_viterbi_one_normal_state(void);
void test_viterbi_two_normal_states(void);
/* void test_viterbi_normal_states(void); */
/* void test_viterbi_profile1(void); */
/* void test_viterbi_profile2(void); */
/* void test_viterbi_profile_delete(void); */
/* void test_viterbi_global_profile(void); */
/* void test_viterbi_table_states(void); */
/* void test_viterbi_cycle_mute_ending(void); */

int main(void)
{
    test_viterbi_one_mute_state();
    test_viterbi_two_mute_states();
    test_viterbi_mute_cycle();
    test_viterbi_one_normal_state();
    test_viterbi_two_normal_states();
    /* test_viterbi_normal_states(); */
    /* test_viterbi_profile1(); */
    /* test_viterbi_profile2(); */
    /* test_viterbi_profile_delete(); */
    /* test_viterbi_global_profile(); */
    /* test_viterbi_table_states(); */
    /* test_viterbi_cycle_mute_ending(); */
    return cass_status();
}

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

    struct imm_result result;
    imm_result_init(&result);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    cass_equal(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(0.5));
    cass_close(result.loglik, imm_log(0.5));

    cass_equal(imm_task_setup(task, C), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, C, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

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

    struct imm_result result;
    imm_result_init(&result);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    cass_equal(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_log(0.5));
    cass_close(result.loglik, imm_log(0.5));

    cass_equal(imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero()),
               IMM_ILLEGALARG);

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.1));

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    cass_equal(imm_task_reset(task, dp), IMM_SUCCESS);

    cass_equal(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 2);
    cass_equal(imm_path_step(&result.path, 0)->state_id, 0);
    cass_equal(imm_path_step(&result.path, 0)->seqlen, 0);
    cass_equal(imm_path_step(&result.path, 1)->state_id, 1);
    cass_equal(imm_path_step(&result.path, 1)->seqlen, 0);
    cass_close(imm_hmm_loglik(hmm, EMPTY, &result.path),
               imm_log(0.5) + imm_log(0.1));
    cass_close(result.loglik, imm_log(0.5) + imm_log(0.1));

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
    cass_null(dp);

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

    struct imm_result result;
    imm_result_init(&result);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state));
    struct imm_task *task = imm_task_new(dp);

    cass_equal(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);

    cass_equal(imm_task_setup(task, A), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, A, &result.path),
               imm_log(1.0) + imm_log(0.25));
    cass_close(result.loglik, imm_log(1.0) + imm_log(0.25));

    cass_equal(imm_task_setup(task, T), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, T, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    cass_equal(imm_task_setup(task, AC), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, AC, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.1));
    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state));
    cass_equal(imm_task_reset(task, dp), IMM_SUCCESS);

    cass_equal(imm_task_setup(task, A), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, A, &result.path),
               imm_log(1.0) + imm_log(0.25));
    cass_close(result.loglik, imm_log(1.0) + imm_log(0.25));

    cass_equal(imm_task_setup(task, AA), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 2);
    cass_close(imm_hmm_loglik(hmm, AA, &result.path),
               imm_log(0.1) + 2 * imm_log(0.25));
    cass_close(result.loglik, imm_log(0.1) + 2 * imm_log(0.25));

    cass_equal(imm_task_setup(task, ACT), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, ACT, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    imm_hmm_normalize_trans(hmm);
    imm_del(dp);
    imm_del(task);
    dp = imm_hmm_new_dp(hmm, imm_super(state));
    task = imm_task_new(dp);

    cass_equal(imm_task_setup(task, A), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, A, &result.path), imm_log(0.25));
    cass_close(result.loglik, imm_log(0.25));

    cass_equal(imm_task_setup(task, AA), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 2);
    cass_close(imm_hmm_loglik(hmm, AA, &result.path), 2 * imm_log(0.25));
    cass_close(result.loglik, 2 * imm_log(0.25));

    cass_equal(imm_task_setup(task, ACT), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, ACT, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

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

    struct imm_result result;
    imm_result_init(&result);

    struct imm_dp *dp = imm_hmm_new_dp(hmm, imm_super(state0));
    struct imm_task *task = imm_task_new(dp);

    cass_equal(imm_task_setup(task, EMPTY), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_ILLEGALARG);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, EMPTY, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    cass_equal(imm_task_setup(task, A), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 1);
    cass_close(imm_hmm_loglik(hmm, A, &result.path),
               imm_log(0.1) + imm_log(0.25));
    cass_close(result.loglik, imm_log(0.1) + imm_log(0.25));

    cass_equal(imm_task_setup(task, T), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, T, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    cass_equal(imm_task_setup(task, AC), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, AC, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    cass_equal(imm_task_reset(task, dp), IMM_SUCCESS);

    cass_equal(imm_task_setup(task, AT), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 2);
    cass_close(imm_hmm_loglik(hmm, AT, &result.path),
               imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));
    cass_close(result.loglik,
               imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5));

    cass_equal(imm_task_setup(task, ATT), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 0);
    cass_close(imm_hmm_loglik(hmm, ATT, &result.path), imm_lprob_invalid());
    cass_close(result.loglik, imm_lprob_invalid());

    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(0.5));
    imm_hmm_set_start(hmm, imm_super(state1), imm_lprob_zero());

    imm_del(dp);
    dp = imm_hmm_new_dp(hmm, imm_super(state1));
    cass_equal(imm_task_reset(task, dp), IMM_SUCCESS);

    cass_equal(imm_task_setup(task, ATT), IMM_SUCCESS);
    cass_equal(imm_dp_viterbi(dp, task, &result), IMM_SUCCESS);
    cass_equal(imm_path_nsteps(&result.path), 3);
    cass_close(imm_hmm_loglik(hmm, ATT, &result.path),
               imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + 3 * imm_log(0.5));
    cass_close(result.loglik,
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

#if 0
void test_viterbi_normal_states(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new("", abc);
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *AA = imm_seq_new("AA", abc);
    struct imm_seq const *AG = imm_seq_new("AG", abc);
    struct imm_seq const *AGT = imm_seq_new("AGT", abc);
    struct imm_seq const *AGTC = imm_seq_new("AGTC", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    imm_float const lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                                 zero()};
    struct imm_normal_state *state0 =
        imm_normal_state_new(0, "State0", abc, lprobs0);

    imm_float const lprobs1[] = {
        imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),

        imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state *state1 =
        imm_normal_state_new(1, "State1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0),
                      imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(state0),
                      imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1),
                      imm_super(state1), imm_log(1.0));

    imm_hmm_normalize_trans(hmm);

    struct imm_path *path = NULL;
    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(state0), &path),
               -1.386294361120);

    cass_close(imm_hmm_loglik(hmm, A, path), -1.386294361120);
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, A, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AG, imm_super(state0), &path),
               -3.178053830348);

    cass_close(imm_hmm_loglik(hmm, AG, path), -3.178053830348);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AG, imm_super(state1), &path),
               -3.295836866004);

    cass_close(imm_hmm_loglik(hmm, AG, path), -3.295836866004);
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, AGT, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, AGT, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AGT, imm_super(state1), &path),
               -4.106767082221);

    cass_close(imm_hmm_loglik(hmm, AGT, path), -4.106767082221);
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, AGTC, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, AGTC, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AGTC, imm_super(state1), &path),
               -6.303991659557);
    cass_close(imm_hmm_loglik(hmm, AGTC, path), -6.303991659557);
    imm_path_destroy(path);

    cass_equal(imm_hmm_set_trans(hmm, imm_super(state0),
                                 imm_super(state0), zero()),
               IMM_ILLEGALARG);
    cass_equal(imm_hmm_set_trans(hmm, imm_super(state0),
                                 imm_super(state1), zero()),
               IMM_ILLEGALARG);
    cass_equal(imm_hmm_set_trans(hmm, imm_super(state1),
                                 imm_super(state0), zero()),
               IMM_ILLEGALARG);
    cass_equal(imm_hmm_set_trans(hmm, imm_super(state1),
                                 imm_super(state1), zero()),
               IMM_ILLEGALARG);

    imm_hmm_set_start(hmm, imm_super(state0), zero());
    imm_hmm_set_start(hmm, imm_super(state1), zero());

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, A, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, imm_super(state0), 0.0);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(state0), &path),
               imm_log(0.25));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, A, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_super(state0),
                      imm_super(state0), imm_log(0.9));

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(state0), &path),
               imm_log(0.25));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, A, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_super(state0), &path),
               2 * imm_log(0.25) + imm_log(0.9));
    cass_close(imm_hmm_loglik(hmm, AA, path),
               2 * imm_log(0.25) + imm_log(0.9));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_super(state0),
                      imm_super(state1), imm_log(0.2));

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(state0), &path),
               imm_log(0.25));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, A, imm_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_super(state0), &path),
               2 * imm_log(0.25) + imm_log(0.9));
    cass_close(imm_hmm_loglik(hmm, AA, path),
               2 * imm_log(0.25) + imm_log(0.9));
    imm_path_destroy(path);

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
    struct imm_abc const *abc = imm_abc_new("AB", '*');
    struct imm_seq const *EMPTY = imm_seq_new("", abc);
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *B = imm_seq_new("B", abc);
    struct imm_seq const *AA = imm_seq_new("AA", abc);
    struct imm_seq const *AAB = imm_seq_new("AAB", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *start = imm_mute_state_new(0, "START", abc);
    struct imm_mute_state *D0 = imm_mute_state_new(1, "D0", abc);
    struct imm_mute_state *end = imm_mute_state_new(2, "END", abc);

    imm_float M0_lprobs[] = {imm_log(0.4), imm_log(0.2)};
    struct imm_normal_state *M0 =
        imm_normal_state_new(3, "M0", abc, M0_lprobs);

    imm_float I0_lprobs[] = {imm_log(0.5), imm_log(0.5)};
    struct imm_normal_state *I0 =
        imm_normal_state_new(4, "I0", abc, I0_lprobs);

    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), 0.0);
    imm_hmm_add_state(hmm, imm_super(D0));
    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_add_state(hmm, imm_super(M0));
    imm_hmm_add_state(hmm, imm_super(I0));

    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(D0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(D0), imm_super(end),
                      imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(M0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(end), imm_log(0.8));
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(I0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(end), imm_log(1.0));

    struct imm_path *path = NULL;
    cass_close(single_viterbi(hmm, EMPTY, imm_super(end), &path),
               imm_log(0.1) + imm_log(1.0));

    cass_close(imm_hmm_loglik(hmm, EMPTY, path),
               imm_log(0.1) + imm_log(1.0));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, EMPTY, imm_super(D0), &path),
               imm_log(0.1));
    cass_close(imm_hmm_loglik(hmm, EMPTY, path), imm_log(0.1));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, EMPTY, imm_super(start), &path),
               imm_log(1.0));
    cass_close(imm_hmm_loglik(hmm, EMPTY, path), imm_log(1.0));
    imm_path_destroy(path);

    cass_cond(!is_valid(
        single_viterbi(hmm, EMPTY, imm_super(M0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(
        !is_valid(single_viterbi(hmm, A, imm_super(start), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(
        !is_valid(single_viterbi(hmm, A, imm_super(D0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(
        !is_valid(single_viterbi(hmm, A, imm_super(I0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(M0), &path),
               imm_log(0.5) + imm_log(0.4));

    cass_close(imm_hmm_loglik(hmm, A, path),
               imm_log(0.5) + imm_log(0.4));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(end), &path),
               imm_log(0.5) + imm_log(0.4) + imm_log(0.8));
    cass_close(imm_hmm_loglik(hmm, A, path),
               imm_log(0.5) + imm_log(0.4) + imm_log(0.8));

    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_super(M0), &path),
               imm_log(0.5) + imm_log(0.2));

    cass_close(imm_hmm_loglik(hmm, B, path),
               imm_log(0.5) + imm_log(0.2));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_super(end), &path),
               imm_log(0.5) + imm_log(0.2) + imm_log(0.8));
    cass_close(imm_hmm_loglik(hmm, B, path),
               imm_log(0.5) + imm_log(0.2) + imm_log(0.8));

    imm_path_destroy(path);

    cass_cond(
        !is_valid(single_viterbi(hmm, AA, imm_super(M0), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, AA, path)));
    imm_path_destroy(path);

    imm_float desired =
        imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AA, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, AA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AA, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, AA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) +
              2 * imm_log(0.5);
    cass_close(single_viterbi(hmm, AAB, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, AAB, path), desired);
    imm_path_destroy(path);

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
    struct imm_abc const *abc = imm_abc_new("ABCD", '*');
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *B = imm_seq_new("B", abc);
    struct imm_seq const *C = imm_seq_new("C", abc);
    struct imm_seq const *D = imm_seq_new("D", abc);
    struct imm_seq const *CA = imm_seq_new("CA", abc);
    struct imm_seq const *CD = imm_seq_new("CD", abc);
    struct imm_seq const *CDDDA = imm_seq_new("CDDDA", abc);
    struct imm_seq const *CDDDAB = imm_seq_new("CDDDAB", abc);
    struct imm_seq const *CDDDABA = imm_seq_new("CDDDABA", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *start = imm_mute_state_new(0, "START", abc);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1),
                              imm_log(0.7)};

    imm_float M0_lprobs[] = {imm_log(0.4), zero(), imm_log(0.6), zero()};
    imm_float M1_lprobs[] = {imm_log(0.6), zero(), imm_log(0.4), zero()};
    imm_float M2_lprobs[] = {imm_log(0.05), imm_log(0.05), imm_log(0.05),
                             imm_log(0.05)};

    struct imm_normal_state *M0 =
        imm_normal_state_new(1, "M0", abc, M0_lprobs);
    struct imm_normal_state *I0 =
        imm_normal_state_new(2, "I0", abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(3, "D1", abc);
    struct imm_normal_state *M1 =
        imm_normal_state_new(4, "M1", abc, M1_lprobs);
    struct imm_normal_state *I1 =
        imm_normal_state_new(5, "I1", abc, ins_lprobs);

    struct imm_mute_state *D2 = imm_mute_state_new(6, "D2", abc);
    struct imm_normal_state *M2 =
        imm_normal_state_new(7, "M2", abc, M2_lprobs);

    struct imm_mute_state *end = imm_mute_state_new(8, "END", abc);

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

    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(M0), 0.0);
    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1),
                      imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1),
                      imm_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M2),
                      imm_super(end), 0.0);

    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(D1),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(D2),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(M1),
                      imm_super(I1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(I1),
                      imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1),
                      imm_super(M2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(D2),
                      imm_log(0.3));
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(M2),
                      imm_log(0.7));

    imm_hmm_set_trans(hmm, imm_super(D2), imm_super(end),
                      imm_log(1.0));

    struct imm_path *path = NULL;
    cass_close(single_viterbi(hmm, A, imm_super(M2), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_super(M2), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, B, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_super(M2), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, C, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, D, imm_super(M2), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, D, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(end), &path),
               imm_log(0.6));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_super(end), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, B, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_super(end), &path),
               imm_log(0.6));
    cass_close(imm_hmm_loglik(hmm, C, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, D, imm_super(end), &path),
               imm_log(0.05));
    cass_close(imm_hmm_loglik(hmm, D, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(M1), &path),
               imm_log(0.6));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_super(M1), &path),
               imm_log(0.4));
    cass_close(imm_hmm_loglik(hmm, C, path), imm_log(0.4));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CA, imm_super(end), &path),
               2 * imm_log(0.6));
    cass_close(imm_hmm_loglik(hmm, CA, path), 2 * imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CD, imm_super(I0), &path),
               imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    cass_close(imm_hmm_loglik(hmm, CD, path),
               imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    imm_path_destroy(path);

    imm_float desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
                        3 * imm_log(0.5) + imm_log(0.6);
    cass_close(single_viterbi(hmm, CDDDA, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CDDDA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
              3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.05);
    cass_close(single_viterbi(hmm, CDDDAB, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CDDDAB, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) +
              3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.2) + imm_log(0.1) +
              imm_log(0.5) + imm_log(0.05);
    cass_close(single_viterbi(hmm, CDDDABA, imm_super(M2), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
              3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, imm_super(M1), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) +
              3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, imm_super(end), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

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
    struct imm_abc const *abc = imm_abc_new("AB", '*');
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *AB = imm_seq_new("AB", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    imm_float N0_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N0 =
        imm_normal_state_new(0, "N0", abc, N0_lprobs);

    struct imm_mute_state *M = imm_mute_state_new(1, "M", abc);

    imm_float N1_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state *N1 =
        imm_normal_state_new(2, "N1", abc, N1_lprobs);

    imm_float N2_lprobs[] = {zero(), imm_log(0.5)};
    struct imm_normal_state *N2 =
        imm_normal_state_new(3, "N2", abc, N2_lprobs);

    imm_hmm_add_state(hmm, imm_super(N2));
    imm_hmm_add_state(hmm, imm_super(N1));
    imm_hmm_add_state(hmm, imm_super(M));
    imm_hmm_add_state(hmm, imm_super(N0));
    imm_hmm_set_start(hmm, imm_super(N0), 0);

    imm_hmm_set_trans(hmm, imm_super(N0),
                      imm_super(N1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(N0), imm_super(M),
                      imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(N1),
                      imm_super(N2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(M), imm_super(N2),
                      imm_log(0.5));

    struct imm_path *path = NULL;
    cass_close(single_viterbi(hmm, A, imm_super(N0), &path),
               imm_log(0.5));
    cass_close(imm_hmm_loglik(hmm, A, path), imm_log(0.5));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(M), &path),
               2 * imm_log(0.5));
    cass_close(imm_hmm_loglik(hmm, A, path), 2 * imm_log(0.5));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AB, imm_super(N2), &path),
               4 * imm_log(0.5));
    cass_close(imm_hmm_loglik(hmm, AB, path), 4 * imm_log(0.5));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_super(M), &path),
               2 * imm_log(0.5));
    cass_close(imm_hmm_loglik(hmm, A, path), 2 * imm_log(0.5));
    imm_path_destroy(path);

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
    struct imm_abc const *abc = imm_abc_new("ABCZ", '*');
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *AA = imm_seq_new("AA", abc);
    struct imm_seq const *AAB = imm_seq_new("AAB", abc);
    struct imm_seq const *C = imm_seq_new("C", abc);
    struct imm_seq const *CC = imm_seq_new("CC", abc);
    struct imm_seq const *CCC = imm_seq_new("CCC", abc);
    struct imm_seq const *CCA = imm_seq_new("CCA", abc);
    struct imm_seq const *CCAB = imm_seq_new("CCAB", abc);
    struct imm_seq const *CCABB = imm_seq_new("CCABB", abc);
    struct imm_seq const *CCABA = imm_seq_new("CCABA", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *start = imm_mute_state_new(0, "START", abc);

    imm_float B_lprobs[] = {imm_log(0.01), imm_log(0.01), imm_log(1.0), zero()};
    struct imm_normal_state *B = imm_normal_state_new(1, "B", abc, B_lprobs);

    imm_float M0_lprobs[] = {imm_log(0.9), imm_log(0.01), imm_log(0.01),
                             zero()};

    struct imm_normal_state *M0 =
        imm_normal_state_new(2, "M0", abc, M0_lprobs);

    imm_float M1_lprobs[] = {imm_log(0.01), imm_log(0.9), zero(), zero()};
    struct imm_normal_state *M1 =
        imm_normal_state_new(3, "M1", abc, M1_lprobs);

    imm_float M2_lprobs[] = {imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state *M2 =
        imm_normal_state_new(4, "M2", abc, M2_lprobs);

    struct imm_mute_state *E = imm_mute_state_new(5, "E", abc);
    struct imm_mute_state *end = imm_mute_state_new(6, "END", abc);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), imm_log(1.0)};
    struct imm_normal_state *Z = imm_normal_state_new(7, "Z", abc, Z_lprobs);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), zero()};
    struct imm_normal_state *I0 =
        imm_normal_state_new(8, "I0", abc, ins_lprobs);
    struct imm_normal_state *I1 =
        imm_normal_state_new(9, "I1", abc, ins_lprobs);

    struct imm_mute_state *D1 = imm_mute_state_new(10, "D1", abc);
    struct imm_mute_state *D2 = imm_mute_state_new(11, "D2", abc);

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

    imm_hmm_set_trans(hmm, imm_super(start),
                      imm_super(B), 0);
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(B),
                      0);
    imm_hmm_set_trans(hmm, imm_super(B),
                      imm_super(M0), 0);
    imm_hmm_set_trans(hmm, imm_super(B),
                      imm_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_super(B),
                      imm_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_super(M1),
                      imm_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_super(M2), imm_super(E),
                      0);
    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(E),
                      0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(E),
                      0);

    imm_hmm_set_trans(hmm, imm_super(E), imm_super(end),
                      0);

    imm_hmm_set_trans(hmm, imm_super(E), imm_super(Z),
                      0);
    imm_hmm_set_trans(hmm, imm_super(Z), imm_super(Z),
                      0);
    imm_hmm_set_trans(hmm, imm_super(Z), imm_super(B),
                      0);

    imm_hmm_set_trans(hmm, imm_super(M0), imm_super(D1),
                      0);
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(D2),
                      0);
    imm_hmm_set_trans(hmm, imm_super(D1), imm_super(M2),
                      0);
    imm_hmm_set_trans(hmm, imm_super(D2), imm_super(E),
                      0);

    imm_hmm_set_trans(hmm, imm_super(M0),
                      imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I0),
                      imm_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_super(M1),
                      imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1),
                      imm_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(I1),
                      imm_super(M2), imm_log(0.5));

    struct imm_path *path = NULL;
    cass_cond(
        !is_valid(single_viterbi(hmm, C, imm_super(start), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, C, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_super(B), &path), 0);
    cass_close(imm_hmm_loglik(hmm, C, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CC, imm_super(B), &path), 0);
    cass_close(imm_hmm_loglik(hmm, CC, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCC, imm_super(B), &path), 0);
    cass_close(imm_hmm_loglik(hmm, CCC, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCA, imm_super(B), &path),
               imm_log(0.01));
    cass_close(imm_hmm_loglik(hmm, CCA, path), imm_log(0.01));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCA, imm_super(M0), &path),
               imm_log(0.9));
    cass_close(imm_hmm_loglik(hmm, CCA, path), imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCAB, imm_super(M1), &path),
               2 * imm_log(0.9));
    cass_close(imm_hmm_loglik(hmm, CCAB, path), 2 * imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCAB, imm_super(I0), &path),
               imm_log(0.9 * 0.5 * 0.1));
    cass_close(imm_hmm_loglik(hmm, CCAB, path),
               imm_log(0.9 * 0.5 * 0.1));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCABB, imm_super(I0), &path),
               imm_log(0.9) + 2 * (imm_log(0.05)));
    cass_close(imm_hmm_loglik(hmm, CCABB, path),
               imm_log(0.9) + 2 * (imm_log(0.05)));

    imm_path_destroy(path);

    imm_float desired = imm_log(0.9) + imm_log(0.5) + imm_log(0.1) +
                        imm_log(0.5) + imm_log(0.01);
    cass_close(single_viterbi(hmm, CCABA, imm_super(M1), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, CCABA, path), desired);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_super(D1), &path),
               imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglik(hmm, AA, path),
               imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_super(D2), &path),
               imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglik(hmm, AA, path),
               imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_super(E), &path),
               imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglik(hmm, AA, path),
               imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    desired = imm_log(0.01) + imm_log(0.9) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AAB, imm_super(M2), &path),
               desired);
    cass_close(imm_hmm_loglik(hmm, AAB, path), desired);
    imm_path_destroy(path);

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

void test_viterbi_table_states(void)
{
    struct imm_abc const *abc = imm_abc_new("ACGTX", '*');
    struct imm_seq const *EMPTY = imm_seq_new("", abc);
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_seq const *seqT = imm_seq_new("T", abc);
    struct imm_seq const *TAT = imm_seq_new("TAT", abc);
    struct imm_seq const *TATX = imm_seq_new("TATX", abc);
    struct imm_seq const *TATA = imm_seq_new("TATA", abc);
    struct imm_seq const *TATTX = imm_seq_new("TATTX", abc);
    struct imm_seq const *TATTXX = imm_seq_new("TATTXX", abc);
    struct imm_seq const *CAXCA = imm_seq_new("CAXCA", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *S = imm_mute_state_new(0, "S", abc);
    struct imm_seq_table *table = imm_seq_table_create(abc);
    imm_seq_table_add(table, EMPTY, imm_log(0.5));
    imm_seq_table_add(table, A, imm_log(0.1));
    imm_seq_table_add(table, TAT, imm_log(0.2));
    cass_cond(imm_seq_table_lprob(table, EMPTY) == imm_log(0.5));
    cass_cond(imm_seq_table_lprob(table, A) == imm_log(0.1));
    cass_cond(imm_seq_table_lprob(table, TAT) == imm_log(0.2));
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seqT)));
    struct imm_table_state *T = imm_table_state_create(1, "T", table);
    imm_seq_table_destroy(table);

    struct imm_mute_state *D = imm_mute_state_new(2, "D", abc);

    imm_float N0_lprobs[] = {zero(), imm_log(0.5), imm_log(0.5), zero(),
                             zero()};
    struct imm_normal_state *N0 =
        imm_normal_state_new(3, "N0", abc, N0_lprobs);

    imm_float N1_lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.25),
                             imm_log(0.25), zero()};
    struct imm_normal_state *N1 =
        imm_normal_state_new(4, "N1", abc, N1_lprobs);

    struct imm_mute_state *E = imm_mute_state_new(5, "E", abc);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), zero(), imm_log(1.0)};

    struct imm_normal_state *Z = imm_normal_state_new(6, "Z", abc, Z_lprobs);

    imm_hmm_add_state(hmm, imm_super(S));
    imm_hmm_set_start(hmm, imm_super(S), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_table_state_super(T));
    imm_hmm_add_state(hmm, imm_super(N0));
    imm_hmm_add_state(hmm, imm_super(D));
    imm_hmm_add_state(hmm, imm_super(N1));
    imm_hmm_add_state(hmm, imm_super(E));
    imm_hmm_add_state(hmm, imm_super(Z));

    cass_cond(imm_hmm_set_trans(hmm, imm_super(S),
                                imm_table_state_super(T),
                                imm_lprob_invalid()) == IMM_ILLEGALARG);
    cass_cond(imm_hmm_set_trans(hmm, imm_super(S),
                                imm_table_state_super(T),
                                imm_lprob_invalid()) == IMM_ILLEGALARG);

    imm_hmm_set_trans(hmm, imm_super(S), imm_table_state_super(T),
                      imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_table_state_super(T), imm_super(D),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_table_state_super(T), imm_super(N0),
                      imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(D), imm_super(N1),
                      imm_log(0.3));
    imm_hmm_set_trans(hmm, imm_super(N0),
                      imm_super(N1), imm_log(0.4));
    imm_hmm_set_trans(hmm, imm_super(N1), imm_super(E),
                      imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(Z),
                      imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_super(Z), imm_super(Z),
                      imm_log(2.0));
    imm_hmm_set_trans(hmm, imm_super(Z), imm_table_state_super(T),
                      imm_log(0.6));

    struct imm_path *path = NULL;
    cass_cond(
        !is_valid(single_viterbi(hmm, TATX, imm_super(E), &path)));
    cass_cond(!is_valid(imm_hmm_loglik(hmm, TATX, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATA, imm_super(N1), &path),
               -6.502290170873972);
    cass_close(imm_hmm_loglik(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATA, imm_super(E), &path),
               -6.502290170873972);
    cass_close(imm_hmm_loglik(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATTX, imm_super(Z), &path),
               -7.195437351433918);
    cass_close(imm_hmm_loglik(hmm, TATTX, path), -7.195437351433918);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATTXX, imm_super(Z), &path),
               -6.502290170873972);
    cass_close(imm_hmm_loglik(hmm, TATTXX, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CAXCA, imm_super(E), &path),
               -11.800607537422009);
    cass_close(imm_hmm_loglik(hmm, CAXCA, path), -11.800607537422009);
    imm_path_destroy(path);

    imm_del(hmm);
    imm_del(S);
    imm_table_state_destroy(T);
    imm_del(D);
    imm_del(N0);
    imm_del(N1);
    imm_del(E);
    imm_del(Z);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(seqT);
    imm_del(TAT);
    imm_del(TATX);
    imm_del(TATA);
    imm_del(TATTX);
    imm_del(TATTXX);
    imm_del(CAXCA);
}

void test_viterbi_cycle_mute_ending(void)
{
    struct imm_abc const *abc = imm_abc_new("AB", '*');
    struct imm_seq const *A = imm_seq_new("A", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *start = imm_mute_state_new(0, "START", abc);
    imm_hmm_add_state(hmm, imm_super(start));
    imm_hmm_set_start(hmm, imm_super(start), imm_log(1.0));

    struct imm_mute_state *B = imm_mute_state_new(1, "B", abc);
    imm_hmm_add_state(hmm, imm_super(B));

    imm_float lprobs[] = {imm_log(0.01), imm_log(0.01)};
    struct imm_normal_state *M = imm_normal_state_new(2, "M", abc, lprobs);
    imm_hmm_add_state(hmm, imm_super(M));

    struct imm_mute_state *E = imm_mute_state_new(3, "E", abc);
    imm_hmm_add_state(hmm, imm_super(E));

    struct imm_mute_state *J = imm_mute_state_new(4, "J", abc);
    imm_hmm_add_state(hmm, imm_super(J));

    struct imm_mute_state *end = imm_mute_state_new(5, "END", abc);
    imm_hmm_add_state(hmm, imm_super(end));

    imm_hmm_set_trans(hmm, imm_super(start), imm_super(B),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(B), imm_super(M),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(M), imm_super(E),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(end),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(J),
                      imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(J), imm_super(B),
                      imm_log(0.1));

    struct imm_path *path = NULL;
    cass_close(single_viterbi(hmm, A, imm_super(end), &path),
               -13.815510557964272);

    imm_path_destroy(path);

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
#endif
