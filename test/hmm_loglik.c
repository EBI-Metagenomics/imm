#include "cass/cass.h"
#include "imm/imm.h"

void test_hmm_loglik_single_state(void);
void test_hmm_loglik_two_states(void);
void test_hmm_loglik_mute_state(void);
void test_hmm_loglik_two_mute_states(void);
void test_hmm_loglik_invalid(void);
void test_hmm_loglik_no_state(void);

int main(void)
{
    test_hmm_loglik_single_state();
    test_hmm_loglik_two_states();
    test_hmm_loglik_mute_state();
    test_hmm_loglik_two_mute_states();
    test_hmm_loglik_invalid();
    test_hmm_loglik_no_state();
    return cass_status();
}

static inline imm_float zero(void) { return imm_lprob_zero(); }
static inline int is_valid(imm_float a) { return imm_lprob_is_valid(a); }

void test_hmm_loglik_single_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *AG = imm_seq_new(2, "AG", abc);
    struct imm_seq const *AA = imm_seq_new(2, "AA", abc);

    imm_float lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};

    struct imm_normal_state *state = imm_normal_state_new(0, abc, lprobs);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    cass_equal(imm_hmm_add_state(hmm, imm_super(state)), IMM_SUCCESS);
    cass_equal(imm_hmm_set_start(hmm, imm_super(state), imm_log(0.5)),
               IMM_SUCCESS);
    cass_equal(imm_hmm_normalize_trans(hmm), IMM_SUCCESS);

    struct imm_path path;
    imm_path_init(&path);
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    cass_close(imm_hmm_loglik(hmm, A, &path), imm_log(0.5) + imm_log(0.25));
    imm_path_reset(&path);

    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, A, &path)));
    imm_path_reset(&path);

    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, EMPTY, &path)));
    imm_path_reset(&path);

    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, A, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, AG, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, AA, &path)));
    imm_path_reset(&path);

    cass_equal(imm_hmm_normalize_trans(hmm), 0);
    cass_equal(
        imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), zero()),
        IMM_ILLEGALARG);
    cass_equal(imm_hmm_normalize_trans(hmm), 0);
    cass_equal(imm_hmm_set_trans(hmm, imm_super(state), imm_super(state),
                                 imm_log(0.5)),
               IMM_SUCCESS);
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    cass_close(imm_hmm_loglik(hmm, AA, &path),
               2 * imm_log(0.5) + 2 * imm_log(0.25));
    imm_path_reset(&path);

    cass_cond(imm_hmm_normalize_trans(hmm) == 0);
    imm_path_reset(&path);
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 1));
    cass_close(imm_hmm_loglik(hmm, AA, &path),
               imm_log(0.5) + 2 * imm_log(0.25));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(AA);
    imm_del(AG);
}

void test_hmm_loglik_two_states(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *T = imm_seq_new(1, "T", abc);
    struct imm_seq const *G = imm_seq_new(1, "G", abc);
    struct imm_seq const *GT = imm_seq_new(2, "GT", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5), zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.5), imm_log(0.25), imm_log(0.5),
                           imm_log(1.0)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(1.0));

    struct imm_path path;
    imm_path_init(&path);
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state0)), 1));
    cass_close(imm_hmm_loglik(hmm, A, &path), -1.3862943611);
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state0)), 1));
    cass_cond(imm_lprob_is_zero(imm_hmm_loglik(hmm, T, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state1)), 1));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, G, &path)));
    imm_path_reset(&path);

    cass_cond(imm_hmm_normalize_trans(hmm) == 0);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state0)), 1));
    cass_close(imm_hmm_loglik(hmm, G, &path), -0.6931471806);
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state0)), 1));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state1)), 1));
    cass_close(imm_hmm_loglik(hmm, GT, &path), -1.0986122887);

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
    imm_del(abc);
    imm_del(A);
    imm_del(T);
    imm_del(G);
    imm_del(GT);
}

void test_hmm_loglik_mute_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_seq const *A = imm_seq_new(1, "A", abc);
    struct imm_seq const *T = imm_seq_new(1, "T", abc);
    struct imm_seq const *G = imm_seq_new(1, "G", abc);
    struct imm_seq const *GT = imm_seq_new(2, "GT", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *state = imm_mute_state_new(0, abc);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.1));

    struct imm_path path;
    imm_path_init(&path);
    cass_cond(!is_valid(imm_hmm_loglik(hmm, A, &path)));
    imm_path_reset(&path);

    cass_cond(!is_valid(imm_hmm_loglik(hmm, T, &path)));
    imm_path_reset(&path);

    cass_cond(!is_valid(imm_hmm_loglik(hmm, G, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, IMM_STEP(imm_id(imm_super(state)), 0));
    cass_close(imm_hmm_loglik(hmm, EMPTY, &path), 0.0);
    imm_path_reset(&path);

    cass_cond(!is_valid(imm_hmm_loglik(hmm, GT, &path)));
    imm_path_reset(&path);

    cass_cond(!is_valid(imm_hmm_loglik(hmm, GT, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state);
    imm_del(abc);
    imm_del(EMPTY);
    imm_del(A);
    imm_del(T);
    imm_del(G);
    imm_del(GT);
}

void test_hmm_loglik_two_mute_states(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *S0 = imm_mute_state_new(0, abc);
    struct imm_mute_state *S1 = imm_mute_state_new(1, abc);

    imm_hmm_add_state(hmm, imm_super(S0));
    imm_hmm_set_start(hmm, imm_super(S0), 0.0);
    imm_hmm_add_state(hmm, imm_super(S1));

    imm_hmm_set_trans(hmm, imm_super(S0), imm_super(S1), 0.0);

    struct imm_path path;
    imm_path_init(&path);
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(S0)), 0));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(S1)), 0));
    cass_close(imm_hmm_loglik(hmm, EMPTY, &path), 0.0);

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(S0);
    imm_del(S1);
    imm_del(abc);
    imm_del(EMPTY);
}

void test_hmm_loglik_invalid(void)
{
    struct imm_abc const *abc = imm_abc_new(2, "AC", '*');
    struct imm_seq const *C = imm_seq_new(1, "C", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_mute_state *S = imm_mute_state_new(0, abc);
    imm_hmm_add_state(hmm, imm_super(S));
    imm_hmm_set_start(hmm, imm_super(S), 0.0);

    struct imm_mute_state *M1 = imm_mute_state_new(1, abc);
    imm_hmm_add_state(hmm, imm_super(M1));

    imm_float const lprobs[] = {imm_log(0.8), imm_log(0.2)};
    struct imm_normal_state *M2 = imm_normal_state_new(2, abc, lprobs);
    imm_hmm_add_state(hmm, imm_super(M2));

    struct imm_mute_state *E = imm_mute_state_new(3, abc);
    imm_hmm_add_state(hmm, imm_super(E));

    imm_hmm_set_trans(hmm, imm_super(S), imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M2), imm_super(E), 0.0);
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(E), 0.0);

    imm_hmm_normalize_trans(hmm);

    struct imm_path path;
    imm_path_init(&path);
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(S)), 0));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(M2)), 1));
    imm_path_add(&path, IMM_STEP(imm_id(imm_super(E)), 0));
    cass_cond(!imm_lprob_is_finite(imm_hmm_loglik(hmm, C, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(S);
    imm_del(M1);
    imm_del(M2);
    imm_del(E);
    imm_del(abc);
    imm_del(C);
}

void test_hmm_loglik_no_state(void)
{
    struct imm_abc const *abc = imm_abc_new(4, "ACGT", '*');
    struct imm_seq const *EMPTY = imm_seq_new(0, "", abc);
    struct imm_hmm *hmm = imm_hmm_new(abc);

    struct imm_path path;
    imm_path_init(&path);
    cass_cond(!is_valid(imm_hmm_loglik(hmm, EMPTY, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(abc);
    imm_del(EMPTY);
}
