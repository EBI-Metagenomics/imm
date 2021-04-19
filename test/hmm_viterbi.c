#include "cass/cass.h"
#include "imm/imm.h"

void test_hmm_viterbi_one_mute_state(void);
void test_hmm_viterbi_two_mute_states(void);
void test_hmm_viterbi_mute_cycle(void);
void test_hmm_viterbi_one_normal_state(void);
void test_hmm_viterbi_two_normal_states(void);
void test_hmm_viterbi_normal_states(void);
void test_hmm_viterbi_profile1(void);
void test_hmm_viterbi_profile2(void);
void test_hmm_viterbi_profile_delete(void);
void test_hmm_viterbi_global_profile(void);
void test_hmm_viterbi_table_states(void);
void test_hmm_viterbi_cycle_mute_ending(void);

int main(void)
{
    test_hmm_viterbi_one_mute_state();
    test_hmm_viterbi_two_mute_states();
    test_hmm_viterbi_mute_cycle();
    test_hmm_viterbi_one_normal_state();
    test_hmm_viterbi_two_normal_states();
    test_hmm_viterbi_normal_states();
    test_hmm_viterbi_profile1();
    test_hmm_viterbi_profile2();
    test_hmm_viterbi_profile_delete();
    test_hmm_viterbi_global_profile();
    test_hmm_viterbi_table_states();
    test_hmm_viterbi_cycle_mute_ending();
    return cass_status();
}

static imm_float single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq, struct imm_state const* end_state,
                                struct imm_path** path);
static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int       is_valid(imm_float a) { return imm_lprob_is_valid(a); }

void test_hmm_viterbi_one_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state = imm_mute_state_create(0, "state", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state));
    imm_hmm_set_start(hmm, imm_mute_state_super(state), imm_log(0.5));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(state), &path), imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.5));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, C, imm_mute_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, imm_mute_state_super(state), imm_lprob_zero());

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_mute_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, C, imm_mute_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(C);
}

void test_hmm_viterbi_two_mute_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create(0, "state", abc);
    struct imm_mute_state const* state1 = imm_mute_state_create(1, "state", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0));
    imm_hmm_set_start(hmm, imm_mute_state_super(state0), imm_log(0.5));
    /* imm_hmm_add_state(hmm, imm_mute_state_super(state1), imm_log(0.1)); */
    imm_hmm_add_state(hmm, imm_mute_state_super(state1));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(state0), &path), imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.5));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, imm_mute_state_super(state1), imm_lprob_zero());

    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(state0), &path), imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.5));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_mute_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_mute_state_super(state1), imm_log(0.1));

    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(state1), &path), imm_log(0.5) + imm_log(0.1));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.5) + imm_log(0.1));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_viterbi_mute_cycle(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state0 = imm_mute_state_create(0, "State0", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state0));
    imm_hmm_set_start(hmm, imm_mute_state_super(state0), imm_log(0.5));

    struct imm_mute_state const* state1 = imm_mute_state_create(1, "State1", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(state1));

    imm_hmm_set_trans(hmm, imm_mute_state_super(state0), imm_mute_state_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_mute_state_super(state1), imm_mute_state_super(state0), imm_log(0.2));

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_mute_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state0);
    imm_mute_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_viterbi_one_normal_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AC = imm_seq_create("AC", abc);
    struct imm_seq const* ACT = imm_seq_create("ACT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    imm_float                      lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state const* state = imm_normal_state_create(0, "State0", abc, lprobs0);

    imm_hmm_add_state(hmm, imm_normal_state_super(state));
    imm_hmm_set_start(hmm, imm_normal_state_super(state), imm_log(0.1));

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state), &path), imm_log(0.1) + imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.1) + imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, T, imm_normal_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, T, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AC, imm_normal_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AC, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_normal_state_super(state), imm_normal_state_super(state), imm_log(0.1));

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state), &path), imm_log(0.1) + imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.1) + imm_log(0.25));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_normal_state_super(state), &path), 2 * imm_log(0.1) + 2 * imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), 2 * imm_log(0.1) + 2 * imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, ACT, imm_normal_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, ACT, path)));
    imm_path_destroy(path);

    imm_hmm_normalize(hmm);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state), &path), imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_normal_state_super(state), &path), 2 * imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), 2 * imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, ACT, imm_normal_state_super(state), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, ACT, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(AA);
    imm_seq_destroy(AC);
    imm_seq_destroy(ACT);
}

void test_hmm_viterbi_two_normal_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* AC = imm_seq_create("AC", abc);
    struct imm_seq const* AT = imm_seq_create("AT", abc);
    struct imm_seq const* ATT = imm_seq_create("ATT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    imm_float                      lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create(0, "State0", abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), imm_log(0.5)};

    struct imm_normal_state const* state1 = imm_normal_state_create(1, "State1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_normal_state_super(state0));
    imm_hmm_set_start(hmm, imm_normal_state_super(state0), imm_log(0.1));
    /* imm_hmm_add_state(hmm, imm_normal_state_super(state1), imm_log(0.2)); */
    imm_hmm_add_state(hmm, imm_normal_state_super(state1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state1), imm_log(0.3));

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state0), &path), imm_log(0.1) + imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.1) + imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, T, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, T, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AC, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AC, path)));
    imm_path_destroy(path);

    imm_float desired = imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AT, imm_normal_state_super(state1), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, AT, path), desired);
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, ATT, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, ATT, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_normal_state_super(state1), imm_normal_state_super(state1), imm_log(0.5));
    imm_hmm_set_start(hmm, imm_normal_state_super(state1), imm_lprob_zero());

    desired = imm_log(0.1) + imm_log(0.25) + imm_log(0.3) + 3 * imm_log(0.5);
    cass_close(single_viterbi(hmm, ATT, imm_normal_state_super(state1), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, ATT, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(AC);
    imm_seq_destroy(AT);
    imm_seq_destroy(ATT);
}

void test_hmm_viterbi_normal_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AG = imm_seq_create("AG", abc);
    struct imm_seq const* AGT = imm_seq_create("AGT", abc);
    struct imm_seq const* AGTC = imm_seq_create("AGTC", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    imm_float const                lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create(0, "State0", abc, lprobs0);

    imm_float const                lprobs1[] = {imm_log(0.5) - imm_log(2.25), imm_log(0.25) - imm_log(2.25),

                                 imm_log(0.5) - imm_log(2.25), imm_log(1.0) - imm_log(2.25)};
    struct imm_normal_state const* state1 = imm_normal_state_create(1, "State1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_normal_state_super(state0));
    imm_hmm_set_start(hmm, imm_normal_state_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_normal_state_super(state1));

    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(state1), imm_normal_state_super(state1), imm_log(1.0));

    imm_hmm_normalize(hmm);

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state0), &path), -1.386294361120);

    cass_close(imm_hmm_loglikelihood(hmm, A, path), -1.386294361120);
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AG, imm_normal_state_super(state0), &path), -3.178053830348);

    cass_close(imm_hmm_loglikelihood(hmm, AG, path), -3.178053830348);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AG, imm_normal_state_super(state1), &path), -3.295836866004);

    cass_close(imm_hmm_loglikelihood(hmm, AG, path), -3.295836866004);
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AGT, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AGT, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AGT, imm_normal_state_super(state1), &path), -4.106767082221);

    cass_close(imm_hmm_loglikelihood(hmm, AGT, path), -4.106767082221);
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AGTC, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AGTC, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AGTC, imm_normal_state_super(state1), &path), -6.303991659557);
    cass_close(imm_hmm_loglikelihood(hmm, AGTC, path), -6.303991659557);
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state0), zero());
    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state1), zero());
    imm_hmm_set_trans(hmm, imm_normal_state_super(state1), imm_normal_state_super(state0), zero());
    imm_hmm_set_trans(hmm, imm_normal_state_super(state1), imm_normal_state_super(state1), zero());

    imm_hmm_set_start(hmm, imm_normal_state_super(state0), zero());
    imm_hmm_set_start(hmm, imm_normal_state_super(state1), zero());

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_start(hmm, imm_normal_state_super(state0), 0.0);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state0), &path), imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state0), imm_log(0.9));

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state0), &path), imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_normal_state_super(state0), &path), 2 * imm_log(0.25) + imm_log(0.9));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), 2 * imm_log(0.25) + imm_log(0.9));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state1), imm_log(0.2));

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(state0), &path), imm_log(0.25));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(state1), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_normal_state_super(state0), &path), 2 * imm_log(0.25) + imm_log(0.9));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), 2 * imm_log(0.25) + imm_log(0.9));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AG);
    imm_seq_destroy(AGT);
    imm_seq_destroy(AGTC);
}

void test_hmm_viterbi_profile1(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* B = imm_seq_create("B", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AAB = imm_seq_create("AAB", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create(0, "START", abc);
    struct imm_mute_state const* D0 = imm_mute_state_create(1, "D0", abc);
    struct imm_mute_state const* end = imm_mute_state_create(2, "END", abc);

    imm_float                      M0_lprobs[] = {imm_log(0.4), imm_log(0.2)};
    struct imm_normal_state const* M0 = imm_normal_state_create(3, "M0", abc, M0_lprobs);

    imm_float                      I0_lprobs[] = {imm_log(0.5), imm_log(0.5)};
    struct imm_normal_state const* I0 = imm_normal_state_create(4, "I0", abc, I0_lprobs);

    imm_hmm_add_state(hmm, imm_mute_state_super(start));
    imm_hmm_set_start(hmm, imm_mute_state_super(start), 0.0);
    imm_hmm_add_state(hmm, imm_mute_state_super(D0));
    imm_hmm_add_state(hmm, imm_mute_state_super(end));

    imm_hmm_add_state(hmm, imm_normal_state_super(M0));
    imm_hmm_add_state(hmm, imm_normal_state_super(I0));

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_mute_state_super(D0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_mute_state_super(D0), imm_mute_state_super(end), imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(M0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_mute_state_super(end), imm_log(0.8));
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(I0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_normal_state_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_mute_state_super(end), imm_log(1.0));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(end), &path), imm_log(0.1) + imm_log(1.0));

    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.1) + imm_log(1.0));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(D0), &path), imm_log(0.1));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(0.1));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, EMPTY, imm_mute_state_super(start), &path), imm_log(1.0));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), imm_log(1.0));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, EMPTY, imm_normal_state_super(M0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_mute_state_super(start), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_mute_state_super(D0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, A, imm_normal_state_super(I0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(M0), &path), imm_log(0.5) + imm_log(0.4));

    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.5) + imm_log(0.4));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_mute_state_super(end), &path), imm_log(0.5) + imm_log(0.4) + imm_log(0.8));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.5) + imm_log(0.4) + imm_log(0.8));

    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_normal_state_super(M0), &path), imm_log(0.5) + imm_log(0.2));

    cass_close(imm_hmm_loglikelihood(hmm, B, path), imm_log(0.5) + imm_log(0.2));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_mute_state_super(end), &path), imm_log(0.5) + imm_log(0.2) + imm_log(0.8));
    cass_close(imm_hmm_loglikelihood(hmm, B, path), imm_log(0.5) + imm_log(0.2) + imm_log(0.8));

    imm_path_destroy(path);

    cass_cond(!is_valid(single_viterbi(hmm, AA, imm_normal_state_super(M0), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    imm_float desired = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AA, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AA, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.5) + imm_log(0.4) + imm_log(0.1) + imm_log(0.2) + 2 * imm_log(0.5);
    cass_close(single_viterbi(hmm, AAB, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, AAB, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(D0);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(B);
    imm_seq_destroy(AA);
    imm_seq_destroy(AAB);
}

void test_hmm_viterbi_profile2(void)
{
    struct imm_abc const* abc = imm_abc_create("ABCD", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* B = imm_seq_create("B", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* D = imm_seq_create("D", abc);
    struct imm_seq const* CA = imm_seq_create("CA", abc);
    struct imm_seq const* CD = imm_seq_create("CD", abc);
    struct imm_seq const* CDDDA = imm_seq_create("CDDDA", abc);
    struct imm_seq const* CDDDAB = imm_seq_create("CDDDAB", abc);
    struct imm_seq const* CDDDABA = imm_seq_create("CDDDABA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create(0, "START", abc);

    imm_float ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), imm_log(0.7)};

    imm_float M0_lprobs[] = {imm_log(0.4), zero(), imm_log(0.6), zero()};
    imm_float M1_lprobs[] = {imm_log(0.6), zero(), imm_log(0.4), zero()};
    imm_float M2_lprobs[] = {imm_log(0.05), imm_log(0.05), imm_log(0.05), imm_log(0.05)};

    struct imm_normal_state const* M0 = imm_normal_state_create(1, "M0", abc, M0_lprobs);
    struct imm_normal_state const* I0 = imm_normal_state_create(2, "I0", abc, ins_lprobs);

    struct imm_mute_state const*   D1 = imm_mute_state_create(3, "D1", abc);
    struct imm_normal_state const* M1 = imm_normal_state_create(4, "M1", abc, M1_lprobs);
    struct imm_normal_state const* I1 = imm_normal_state_create(5, "I1", abc, ins_lprobs);

    struct imm_mute_state const*   D2 = imm_mute_state_create(6, "D2", abc);
    struct imm_normal_state const* M2 = imm_normal_state_create(7, "M2", abc, M2_lprobs);

    struct imm_mute_state const* end = imm_mute_state_create(8, "END", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(start));
    imm_hmm_set_start(hmm, imm_mute_state_super(start), 0.0);

    imm_hmm_add_state(hmm, imm_normal_state_super(M0));
    imm_hmm_add_state(hmm, imm_normal_state_super(I0));

    imm_hmm_add_state(hmm, imm_mute_state_super(D1));
    imm_hmm_add_state(hmm, imm_normal_state_super(M1));
    imm_hmm_add_state(hmm, imm_normal_state_super(I1));

    imm_hmm_add_state(hmm, imm_mute_state_super(D2));
    imm_hmm_add_state(hmm, imm_normal_state_super(M2));

    imm_hmm_add_state(hmm, imm_mute_state_super(end));

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(M0), 0.0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_mute_state_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_normal_state_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_mute_state_super(end), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M2), imm_mute_state_super(end), 0.0);

    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(I0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_mute_state_super(D1), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_normal_state_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_normal_state_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_mute_state_super(D2), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_normal_state_super(I1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I1), imm_normal_state_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I1), imm_normal_state_super(M2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_mute_state_super(D1), imm_mute_state_super(D2), imm_log(0.3));
    imm_hmm_set_trans(hmm, imm_mute_state_super(D1), imm_normal_state_super(M2), imm_log(0.7));

    imm_hmm_set_trans(hmm, imm_mute_state_super(D2), imm_mute_state_super(end), imm_log(1.0));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, A, imm_normal_state_super(M2), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_normal_state_super(M2), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, B, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_normal_state_super(M2), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, C, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, D, imm_normal_state_super(M2), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, D, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_mute_state_super(end), &path), imm_log(0.6));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, B, imm_mute_state_super(end), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, B, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_mute_state_super(end), &path), imm_log(0.6));
    cass_close(imm_hmm_loglikelihood(hmm, C, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, D, imm_mute_state_super(end), &path), imm_log(0.05));
    cass_close(imm_hmm_loglikelihood(hmm, D, path), imm_log(0.05));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_normal_state_super(M1), &path), imm_log(0.6));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_normal_state_super(M1), &path), imm_log(0.4));
    cass_close(imm_hmm_loglikelihood(hmm, C, path), imm_log(0.4));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CA, imm_mute_state_super(end), &path), 2 * imm_log(0.6));
    cass_close(imm_hmm_loglikelihood(hmm, CA, path), 2 * imm_log(0.6));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CD, imm_normal_state_super(I0), &path), imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    cass_close(imm_hmm_loglikelihood(hmm, CD, path), imm_log(0.6) + imm_log(0.2) + imm_log(0.7));
    imm_path_destroy(path);

    imm_float desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) + imm_log(0.6);
    cass_close(single_viterbi(hmm, CDDDA, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CDDDA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.05);
    cass_close(single_viterbi(hmm, CDDDAB, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CDDDAB, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 3 * imm_log(0.7) + 3 * imm_log(0.5) + imm_log(0.6) + imm_log(0.2) +
              imm_log(0.1) + imm_log(0.5) + imm_log(0.05);
    cass_close(single_viterbi(hmm, CDDDABA, imm_normal_state_super(M2), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) + 3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, imm_normal_state_super(M1), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    desired = imm_log(0.6) + imm_log(0.2) + 5 * imm_log(0.5) + 3 * imm_log(0.7) + 2 * imm_log(0.1) + imm_log(0.6);

    cass_close(single_viterbi(hmm, CDDDABA, imm_mute_state_super(end), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CDDDABA, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(I0);
    imm_mute_state_destroy(D1);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(M2);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(B);
    imm_seq_destroy(C);
    imm_seq_destroy(D);
    imm_seq_destroy(CA);
    imm_seq_destroy(CD);
    imm_seq_destroy(CDDDA);
    imm_seq_destroy(CDDDAB);
    imm_seq_destroy(CDDDABA);
}

void test_hmm_viterbi_profile_delete(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AB = imm_seq_create("AB", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    imm_float                      N0_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state const* N0 = imm_normal_state_create(0, "N0", abc, N0_lprobs);

    struct imm_mute_state const* M = imm_mute_state_create(1, "M", abc);

    imm_float                      N1_lprobs[] = {imm_log(0.5), zero()};
    struct imm_normal_state const* N1 = imm_normal_state_create(2, "N1", abc, N1_lprobs);

    imm_float                      N2_lprobs[] = {zero(), imm_log(0.5)};
    struct imm_normal_state const* N2 = imm_normal_state_create(3, "N2", abc, N2_lprobs);

    imm_hmm_add_state(hmm, imm_normal_state_super(N2));
    imm_hmm_add_state(hmm, imm_normal_state_super(N1));
    imm_hmm_add_state(hmm, imm_mute_state_super(M));
    imm_hmm_add_state(hmm, imm_normal_state_super(N0));
    imm_hmm_set_start(hmm, imm_normal_state_super(N0), 0);

    imm_hmm_set_trans(hmm, imm_normal_state_super(N0), imm_normal_state_super(N1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(N0), imm_mute_state_super(M), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(N1), imm_normal_state_super(N2), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_mute_state_super(M), imm_normal_state_super(N2), imm_log(0.5));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, A, imm_normal_state_super(N0), &path), imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), imm_log(0.5));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, A, imm_mute_state_super(M), &path), 2 * imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), 2 * imm_log(0.5));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AB, imm_normal_state_super(N2), &path), 4 * imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, AB, path), 4 * imm_log(0.5));
    imm_path_destroy(path);

    cass_cond(imm_hmm_del_state(hmm, imm_normal_state_super(N2)) == 0);

    cass_close(single_viterbi(hmm, A, imm_mute_state_super(M), &path), 2 * imm_log(0.5));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), 2 * imm_log(0.5));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(N0);
    imm_mute_state_destroy(M);
    imm_normal_state_destroy(N1);
    imm_normal_state_destroy(N2);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(AB);
}

void test_hmm_viterbi_global_profile(void)
{
    struct imm_abc const* abc = imm_abc_create("ABCZ", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);
    struct imm_seq const* AAB = imm_seq_create("AAB", abc);
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_seq const* CC = imm_seq_create("CC", abc);
    struct imm_seq const* CCC = imm_seq_create("CCC", abc);
    struct imm_seq const* CCA = imm_seq_create("CCA", abc);
    struct imm_seq const* CCAB = imm_seq_create("CCAB", abc);
    struct imm_seq const* CCABB = imm_seq_create("CCABB", abc);
    struct imm_seq const* CCABA = imm_seq_create("CCABA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create(0, "START", abc);

    imm_float                      B_lprobs[] = {imm_log(0.01), imm_log(0.01), imm_log(1.0), zero()};
    struct imm_normal_state const* B = imm_normal_state_create(1, "B", abc, B_lprobs);

    imm_float M0_lprobs[] = {imm_log(0.9), imm_log(0.01), imm_log(0.01), zero()};

    struct imm_normal_state const* M0 = imm_normal_state_create(2, "M0", abc, M0_lprobs);

    imm_float                      M1_lprobs[] = {imm_log(0.01), imm_log(0.9), zero(), zero()};
    struct imm_normal_state const* M1 = imm_normal_state_create(3, "M1", abc, M1_lprobs);

    imm_float                      M2_lprobs[] = {imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state const* M2 = imm_normal_state_create(4, "M2", abc, M2_lprobs);

    struct imm_mute_state const* E = imm_mute_state_create(5, "E", abc);
    struct imm_mute_state const* end = imm_mute_state_create(6, "END", abc);

    imm_float                      Z_lprobs[] = {zero(), zero(), zero(), imm_log(1.0)};
    struct imm_normal_state const* Z = imm_normal_state_create(7, "Z", abc, Z_lprobs);

    imm_float                      ins_lprobs[] = {imm_log(0.1), imm_log(0.1), imm_log(0.1), zero()};
    struct imm_normal_state const* I0 = imm_normal_state_create(8, "I0", abc, ins_lprobs);
    struct imm_normal_state const* I1 = imm_normal_state_create(9, "I1", abc, ins_lprobs);

    struct imm_mute_state const* D1 = imm_mute_state_create(10, "D1", abc);
    struct imm_mute_state const* D2 = imm_mute_state_create(11, "D2", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(start));
    imm_hmm_set_start(hmm, imm_mute_state_super(start), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_normal_state_super(B));
    imm_hmm_add_state(hmm, imm_normal_state_super(M0));
    imm_hmm_add_state(hmm, imm_normal_state_super(M1));
    imm_hmm_add_state(hmm, imm_normal_state_super(M2));
    imm_hmm_add_state(hmm, imm_mute_state_super(D1));
    imm_hmm_add_state(hmm, imm_mute_state_super(D2));
    imm_hmm_add_state(hmm, imm_normal_state_super(I0));
    imm_hmm_add_state(hmm, imm_normal_state_super(I1));
    imm_hmm_add_state(hmm, imm_mute_state_super(E));
    imm_hmm_add_state(hmm, imm_normal_state_super(Z));
    imm_hmm_add_state(hmm, imm_mute_state_super(end));

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_normal_state_super(B), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(B), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M0), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(B), imm_normal_state_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(M1), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_normal_state_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M2), imm_mute_state_super(E), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_mute_state_super(E), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_mute_state_super(E), 0);

    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_mute_state_super(end), 0);

    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_normal_state_super(Z), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(Z), imm_normal_state_super(Z), 0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(Z), imm_normal_state_super(B), 0);

    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_mute_state_super(D1), 0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(D1), imm_mute_state_super(D2), 0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(D1), imm_normal_state_super(M2), 0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(D2), imm_mute_state_super(E), 0);

    imm_hmm_set_trans(hmm, imm_normal_state_super(M0), imm_normal_state_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_normal_state_super(I0), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I0), imm_normal_state_super(M1), imm_log(0.5));

    imm_hmm_set_trans(hmm, imm_normal_state_super(M1), imm_normal_state_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I1), imm_normal_state_super(I1), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(I1), imm_normal_state_super(M2), imm_log(0.5));

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, C, imm_mute_state_super(start), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, C, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, C, imm_normal_state_super(B), &path), 0);
    cass_close(imm_hmm_loglikelihood(hmm, C, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CC, imm_normal_state_super(B), &path), 0);
    cass_close(imm_hmm_loglikelihood(hmm, CC, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCC, imm_normal_state_super(B), &path), 0);
    cass_close(imm_hmm_loglikelihood(hmm, CCC, path), 0);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCA, imm_normal_state_super(B), &path), imm_log(0.01));
    cass_close(imm_hmm_loglikelihood(hmm, CCA, path), imm_log(0.01));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCA, imm_normal_state_super(M0), &path), imm_log(0.9));
    cass_close(imm_hmm_loglikelihood(hmm, CCA, path), imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCAB, imm_normal_state_super(M1), &path), 2 * imm_log(0.9));
    cass_close(imm_hmm_loglikelihood(hmm, CCAB, path), 2 * imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCAB, imm_normal_state_super(I0), &path), imm_log(0.9 * 0.5 * 0.1));
    cass_close(imm_hmm_loglikelihood(hmm, CCAB, path), imm_log(0.9 * 0.5 * 0.1));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CCABB, imm_normal_state_super(I0), &path), imm_log(0.9) + 2 * (imm_log(0.05)));
    cass_close(imm_hmm_loglikelihood(hmm, CCABB, path), imm_log(0.9) + 2 * (imm_log(0.05)));

    imm_path_destroy(path);

    imm_float desired = imm_log(0.9) + imm_log(0.5) + imm_log(0.1) + imm_log(0.5) + imm_log(0.01);
    cass_close(single_viterbi(hmm, CCABA, imm_normal_state_super(M1), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, CCABA, path), desired);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_mute_state_super(D1), &path), imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglikelihood(hmm, AA, path), imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_mute_state_super(D2), &path), imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglikelihood(hmm, AA, path), imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, AA, imm_mute_state_super(E), &path), imm_log(0.01) + imm_log(0.9));

    cass_close(imm_hmm_loglikelihood(hmm, AA, path), imm_log(0.01) + imm_log(0.9));
    imm_path_destroy(path);

    desired = imm_log(0.01) + imm_log(0.9) + imm_log(0.5);
    cass_close(single_viterbi(hmm, AAB, imm_normal_state_super(M2), &path), desired);
    cass_close(imm_hmm_loglikelihood(hmm, AAB, path), desired);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_normal_state_destroy(B);
    imm_normal_state_destroy(M0);
    imm_normal_state_destroy(M1);
    imm_normal_state_destroy(M2);
    imm_normal_state_destroy(I0);
    imm_normal_state_destroy(I1);
    imm_mute_state_destroy(D1);
    imm_mute_state_destroy(D2);
    imm_normal_state_destroy(Z);
    imm_mute_state_destroy(E);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AAB);
    imm_seq_destroy(C);
    imm_seq_destroy(CC);
    imm_seq_destroy(CCC);
    imm_seq_destroy(CCA);
    imm_seq_destroy(CCAB);
    imm_seq_destroy(CCABB);
    imm_seq_destroy(CCABA);
}

void test_hmm_viterbi_table_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGTX", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* seqT = imm_seq_create("T", abc);
    struct imm_seq const* TAT = imm_seq_create("TAT", abc);
    struct imm_seq const* TATX = imm_seq_create("TATX", abc);
    struct imm_seq const* TATA = imm_seq_create("TATA", abc);
    struct imm_seq const* TATTX = imm_seq_create("TATTX", abc);
    struct imm_seq const* TATTXX = imm_seq_create("TATTXX", abc);
    struct imm_seq const* CAXCA = imm_seq_create("CAXCA", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S = imm_mute_state_create(0, "S", abc);
    struct imm_seq_table*        table = imm_seq_table_create(abc);
    imm_seq_table_add(table, EMPTY, imm_log(0.5));
    imm_seq_table_add(table, A, imm_log(0.1));
    imm_seq_table_add(table, TAT, imm_log(0.2));
    cass_cond(imm_seq_table_lprob(table, EMPTY) == imm_log(0.5));
    cass_cond(imm_seq_table_lprob(table, A) == imm_log(0.1));
    cass_cond(imm_seq_table_lprob(table, TAT) == imm_log(0.2));
    cass_cond(imm_lprob_is_zero(imm_seq_table_lprob(table, seqT)));
    struct imm_table_state const* T = imm_table_state_create(1, "T", table);
    imm_seq_table_destroy(table);

    struct imm_mute_state const* D = imm_mute_state_create(2, "D", abc);

    imm_float                      N0_lprobs[] = {zero(), imm_log(0.5), imm_log(0.5), zero(), zero()};
    struct imm_normal_state const* N0 = imm_normal_state_create(3, "N0", abc, N0_lprobs);

    imm_float                      N1_lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.25), imm_log(0.25), zero()};
    struct imm_normal_state const* N1 = imm_normal_state_create(4, "N1", abc, N1_lprobs);

    struct imm_mute_state const* E = imm_mute_state_create(5, "E", abc);

    imm_float Z_lprobs[] = {zero(), zero(), zero(), zero(), imm_log(1.0)};

    struct imm_normal_state const* Z = imm_normal_state_create(6, "Z", abc, Z_lprobs);

    imm_hmm_add_state(hmm, imm_mute_state_super(S));
    imm_hmm_set_start(hmm, imm_mute_state_super(S), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_table_state_super(T));
    imm_hmm_add_state(hmm, imm_normal_state_super(N0));
    imm_hmm_add_state(hmm, imm_mute_state_super(D));
    imm_hmm_add_state(hmm, imm_normal_state_super(N1));
    imm_hmm_add_state(hmm, imm_mute_state_super(E));
    imm_hmm_add_state(hmm, imm_normal_state_super(Z));

    cass_cond(imm_hmm_set_trans(hmm, imm_mute_state_super(S), imm_table_state_super(T), imm_lprob_invalid()) ==
              IMM_ILLEGALARG);
    cass_cond(imm_hmm_set_trans(hmm, imm_mute_state_super(S), imm_table_state_super(T), imm_lprob_invalid()) ==
              IMM_ILLEGALARG);

    imm_hmm_set_trans(hmm, imm_mute_state_super(S), imm_table_state_super(T), imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_table_state_super(T), imm_mute_state_super(D), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_table_state_super(T), imm_normal_state_super(N0), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_mute_state_super(D), imm_normal_state_super(N1), imm_log(0.3));
    imm_hmm_set_trans(hmm, imm_normal_state_super(N0), imm_normal_state_super(N1), imm_log(0.4));
    imm_hmm_set_trans(hmm, imm_normal_state_super(N1), imm_mute_state_super(E), imm_log(1.0));
    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_normal_state_super(Z), imm_log(0.5));
    imm_hmm_set_trans(hmm, imm_normal_state_super(Z), imm_normal_state_super(Z), imm_log(2.0));
    imm_hmm_set_trans(hmm, imm_normal_state_super(Z), imm_table_state_super(T), imm_log(0.6));

    struct imm_path* path = NULL;
    cass_cond(!is_valid(single_viterbi(hmm, TATX, imm_mute_state_super(E), &path)));
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, TATX, path)));
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATA, imm_normal_state_super(N1), &path), -6.502290170873972);
    cass_close(imm_hmm_loglikelihood(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATA, imm_mute_state_super(E), &path), -6.502290170873972);
    cass_close(imm_hmm_loglikelihood(hmm, TATA, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATTX, imm_normal_state_super(Z), &path), -7.195437351433918);
    cass_close(imm_hmm_loglikelihood(hmm, TATTX, path), -7.195437351433918);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, TATTXX, imm_normal_state_super(Z), &path), -6.502290170873972);
    cass_close(imm_hmm_loglikelihood(hmm, TATTXX, path), -6.502290170873972);
    imm_path_destroy(path);

    cass_close(single_viterbi(hmm, CAXCA, imm_mute_state_super(E), &path), -11.800607537422009);
    cass_close(imm_hmm_loglikelihood(hmm, CAXCA, path), -11.800607537422009);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S);
    imm_table_state_destroy(T);
    imm_mute_state_destroy(D);
    imm_normal_state_destroy(N0);
    imm_normal_state_destroy(N1);
    imm_mute_state_destroy(E);
    imm_normal_state_destroy(Z);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(seqT);
    imm_seq_destroy(TAT);
    imm_seq_destroy(TATX);
    imm_seq_destroy(TATA);
    imm_seq_destroy(TATTX);
    imm_seq_destroy(TATTXX);
    imm_seq_destroy(CAXCA);
}

void test_hmm_viterbi_cycle_mute_ending(void)
{
    struct imm_abc const* abc = imm_abc_create("AB", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* start = imm_mute_state_create(0, "START", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(start));
    imm_hmm_set_start(hmm, imm_mute_state_super(start), imm_log(1.0));

    struct imm_mute_state const* B = imm_mute_state_create(1, "B", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(B));

    imm_float                      lprobs[] = {imm_log(0.01), imm_log(0.01)};
    struct imm_normal_state const* M = imm_normal_state_create(2, "M", abc, lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(M));

    struct imm_mute_state const* E = imm_mute_state_create(3, "E", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(E));

    struct imm_mute_state const* J = imm_mute_state_create(4, "J", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(J));

    struct imm_mute_state const* end = imm_mute_state_create(5, "END", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(end));

    imm_hmm_set_trans(hmm, imm_mute_state_super(start), imm_mute_state_super(B), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_mute_state_super(B), imm_normal_state_super(M), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(M), imm_mute_state_super(E), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_mute_state_super(end), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_mute_state_super(J), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_mute_state_super(J), imm_mute_state_super(B), imm_log(0.1));

    struct imm_path* path = NULL;
    cass_close(single_viterbi(hmm, A, imm_mute_state_super(end), &path), -13.815510557964272);

    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(start);
    imm_mute_state_destroy(B);
    imm_normal_state_destroy(M);
    imm_mute_state_destroy(E);
    imm_mute_state_destroy(J);
    imm_mute_state_destroy(end);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
}

static imm_float single_viterbi(struct imm_hmm const* hmm, struct imm_seq const* seq, struct imm_state const* end_state,
                                struct imm_path** path)
{
    struct imm_dp const* dp = imm_hmm_create_dp(hmm, end_state);
    if (dp == NULL) {
        *path = imm_path_create();
        return imm_lprob_invalid();
    }

    struct imm_dp_task* task = imm_dp_task_create(dp);
    imm_dp_task_setup(task, seq);
    struct imm_result const* r = imm_dp_viterbi(dp, task);
    imm_dp_task_destroy(task);
    if (r == NULL) {
        *path = imm_path_create();
        imm_dp_destroy(dp);
        return imm_lprob_invalid();
    }

    *path = imm_path_clone(imm_result_path(r));

    imm_float score = imm_hmm_loglikelihood(hmm, seq, *path);
    imm_result_destroy(r);
    imm_dp_destroy(dp);

    return score;
}
