#include "cass/cass.h"
#include "imm/imm.h"

void test_hmm_likelihood_single_state(void);
void test_hmm_likelihood_two_states(void);
void test_hmm_likelihood_mute_state(void);
void test_hmm_likelihood_two_mute_states(void);
void test_hmm_likelihood_invalid(void);
void test_hmm_likelihood_no_state(void);

int main(void)
{
    test_hmm_likelihood_single_state();
    test_hmm_likelihood_two_states();
    test_hmm_likelihood_mute_state();
    test_hmm_likelihood_two_mute_states();
    test_hmm_likelihood_invalid();
    test_hmm_likelihood_no_state();
    return cass_status();
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int       is_valid(imm_float a) { return imm_lprob_is_valid(a); }

void test_hmm_likelihood_single_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* AG = imm_seq_create("AG", abc);
    struct imm_seq const* AA = imm_seq_create("AA", abc);

    imm_float lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};

    struct imm_normal_state const* state = imm_normal_state_create(0, "State0", abc, lprobs);
    struct imm_hmm*                hmm = imm_hmm_create(abc);

    imm_hmm_add_state(hmm, imm_normal_state_super(state));
    imm_hmm_set_start(hmm, imm_normal_state_super(state), imm_log(0.5));
    cass_cond(imm_hmm_normalize_trans(hmm) == IMM_SUCCESS);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), -2.0794415474);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, AG, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, AA, path)));
    imm_path_destroy(path);

    cass_equal(imm_hmm_normalize_trans(hmm), 0);
    cass_equal(imm_hmm_set_trans(hmm, imm_normal_state_super(state), imm_normal_state_super(state), zero()),
               IMM_ILLEGALARG);
    cass_equal(imm_hmm_normalize_trans(hmm), 0);
    cass_equal(imm_hmm_set_trans(hmm, imm_normal_state_super(state), imm_normal_state_super(state), imm_log(0.5)),
               IMM_SUCCESS);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), 2 * imm_log(0.5) + 2 * imm_log(0.25));
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize_trans(hmm) == 0);
    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    imm_path_append(path, imm_step_create(imm_normal_state_super(state), 1));
    cass_close(imm_hmm_loglikelihood(hmm, AA, path), -3.4657359123);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(AA);
    imm_seq_destroy(AG);
}

void test_hmm_likelihood_two_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* GT = imm_seq_create("GT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    imm_float                      lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state const* state0 = imm_normal_state_create(0, "State0", abc, lprobs0);

    imm_float                      lprobs1[] = {imm_log(0.5), imm_log(0.25), imm_log(0.5), imm_log(1.0)};
    struct imm_normal_state const* state1 = imm_normal_state_create(1, "State1", abc, lprobs1);

    imm_hmm_add_state(hmm, imm_normal_state_super(state0));
    imm_hmm_set_start(hmm, imm_normal_state_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_normal_state_super(state1));

    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_normal_state_super(state0), imm_normal_state_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_normal_state_super(state1), imm_normal_state_super(state1), imm_log(1.0));

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state0), 1));
    cass_close(imm_hmm_loglikelihood(hmm, A, path), -1.3862943611);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state0), 1));
    cass_cond(imm_lprob_is_zero(imm_hmm_loglikelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state1), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, G, path)));
    imm_path_destroy(path);

    cass_cond(imm_hmm_normalize_trans(hmm) == 0);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state0), 1));
    cass_close(imm_hmm_loglikelihood(hmm, G, path), -0.6931471806);
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_normal_state_super(state0), 1));
    imm_path_append(path, imm_step_create(imm_normal_state_super(state1), 1));
    cass_close(imm_hmm_loglikelihood(hmm, GT, path), -1.0986122887);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_normal_state_destroy(state0);
    imm_normal_state_destroy(state1);
    imm_abc_destroy(abc);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(G);
    imm_seq_destroy(GT);
}

void test_hmm_likelihood_mute_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_seq const* A = imm_seq_create("A", abc);
    struct imm_seq const* T = imm_seq_create("T", abc);
    struct imm_seq const* G = imm_seq_create("G", abc);
    struct imm_seq const* GT = imm_seq_create("GT", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* state = imm_mute_state_create(0, "State0", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(state));
    imm_hmm_set_start(hmm, imm_mute_state_super(state), imm_log(1.0));

    imm_hmm_set_trans(hmm, imm_mute_state_super(state), imm_mute_state_super(state), imm_log(0.1));

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, A, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, T, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, G, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_mute_state_super(state), 0));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), 0.0);
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, GT, path)));
    imm_path_destroy(path);

    path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, GT, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(state);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
    imm_seq_destroy(A);
    imm_seq_destroy(T);
    imm_seq_destroy(G);
    imm_seq_destroy(GT);
}

void test_hmm_likelihood_two_mute_states(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S0 = imm_mute_state_create(0, "S0", abc);
    struct imm_mute_state const* S1 = imm_mute_state_create(1, "S1", abc);

    imm_hmm_add_state(hmm, imm_mute_state_super(S0));
    imm_hmm_set_start(hmm, imm_mute_state_super(S0), 0.0);
    imm_hmm_add_state(hmm, imm_mute_state_super(S1));

    imm_hmm_set_trans(hmm, imm_mute_state_super(S0), imm_mute_state_super(S1), 0.0);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_mute_state_super(S0), 0));
    imm_path_append(path, imm_step_create(imm_mute_state_super(S1), 0));
    cass_close(imm_hmm_loglikelihood(hmm, EMPTY, path), 0.0);
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S0);
    imm_mute_state_destroy(S1);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}

void test_hmm_likelihood_invalid(void)
{
    struct imm_abc const* abc = imm_abc_create("AC", '*');
    struct imm_seq const* C = imm_seq_create("C", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_mute_state const* S = imm_mute_state_create(0, "S", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(S));
    imm_hmm_set_start(hmm, imm_mute_state_super(S), 0.0);

    struct imm_mute_state const* M1 = imm_mute_state_create(1, "M1", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(M1));

    imm_float const                lprobs[] = {imm_log(0.8), imm_log(0.2)};
    struct imm_normal_state const* M2 = imm_normal_state_create(2, "M2", abc, lprobs);
    imm_hmm_add_state(hmm, imm_normal_state_super(M2));

    struct imm_mute_state const* E = imm_mute_state_create(3, "E", abc);
    imm_hmm_add_state(hmm, imm_mute_state_super(E));

    imm_hmm_set_trans(hmm, imm_mute_state_super(S), imm_mute_state_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(M1), imm_normal_state_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_normal_state_super(M2), imm_mute_state_super(E), 0.0);
    imm_hmm_set_trans(hmm, imm_mute_state_super(E), imm_mute_state_super(E), 0.0);

    imm_hmm_normalize_trans(hmm);

    struct imm_path* path = imm_path_create();
    imm_path_append(path, imm_step_create(imm_mute_state_super(S), 0));
    cass_cond(imm_step_create(imm_mute_state_super(M1), 1) == NULL);
    imm_path_append(path, imm_step_create(imm_normal_state_super(M2), 1));
    imm_path_append(path, imm_step_create(imm_mute_state_super(E), 0));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglikelihood(hmm, C, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_mute_state_destroy(S);
    imm_mute_state_destroy(M1);
    imm_normal_state_destroy(M2);
    imm_mute_state_destroy(E);
    imm_abc_destroy(abc);
    imm_seq_destroy(C);
}

void test_hmm_likelihood_no_state(void)
{
    struct imm_abc const* abc = imm_abc_create("ACGT", '*');
    struct imm_seq const* EMPTY = imm_seq_create("", abc);
    struct imm_hmm*       hmm = imm_hmm_create(abc);

    struct imm_path* path = imm_path_create();
    cass_cond(!is_valid(imm_hmm_loglikelihood(hmm, EMPTY, path)));
    imm_path_destroy(path);

    imm_hmm_destroy(hmm);
    imm_abc_destroy(abc);
    imm_seq_destroy(EMPTY);
}
