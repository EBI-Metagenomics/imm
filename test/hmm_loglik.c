#include "hope/hope.h"
#include "imm/imm.h"

void test_hmm_loglik_single_state(void);
void test_hmm_loglik_two_states(void);
void test_hmm_loglik_mute_state(void);
void test_hmm_loglik_two_mute_states(void);
void test_hmm_loglik_invalid(void);
void test_hmm_loglik_no_state(void);

static struct imm_abc abc;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq T;
static struct imm_seq G;
static struct imm_seq AG;
static struct imm_seq AA;
static struct imm_seq GT;

int main(void)
{
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');
    imm_seq_init(&EMPTY, IMM_STR(""), &abc);
    imm_seq_init(&A, IMM_STR("A"), &abc);
    imm_seq_init(&T, IMM_STR("T"), &abc);
    imm_seq_init(&G, IMM_STR("G"), &abc);
    imm_seq_init(&AG, IMM_STR("AG"), &abc);
    imm_seq_init(&AA, IMM_STR("AA"), &abc);
    imm_seq_init(&GT, IMM_STR("GT"), &abc);

    test_hmm_loglik_single_state();
    test_hmm_loglik_two_states();
    test_hmm_loglik_mute_state();
    test_hmm_loglik_two_mute_states();
    test_hmm_loglik_invalid();
    test_hmm_loglik_no_state();
    return hope_status();
}

void test_hmm_loglik_single_state(void)
{
    imm_float lprobs[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                          imm_lprob_zero()};

    struct imm_normal_state *state = imm_normal_state_new(0, &abc, lprobs);
    struct imm_hmm *hmm = imm_hmm_new(&abc);

    EQ(imm_hmm_add_state(hmm, imm_super(state)), IMM_SUCCESS);
    EQ(imm_hmm_set_start(hmm, imm_super(state), imm_log(0.5)), IMM_SUCCESS);
    EQ(imm_hmm_normalize_trans(hmm), IMM_SUCCESS);

    struct imm_path path = imm_path_init();
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    CLOSE(imm_hmm_loglik(hmm, &A, &path), imm_log(0.5) + imm_log(0.25));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &A, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &EMPTY, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &A, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &AG, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &AA, &path)));
    imm_path_reset(&path);

    EQ(imm_hmm_normalize_trans(hmm), 0);
    EQ(imm_hmm_set_trans(hmm, imm_super(state), imm_super(state),
                         imm_lprob_zero()),
       IMM_ILLEGALARG);
    EQ(imm_hmm_normalize_trans(hmm), 0);
    EQ(imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.5)),
       IMM_SUCCESS);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    CLOSE(imm_hmm_loglik(hmm, &AA, &path),
          2 * imm_log(0.5) + 2 * imm_log(0.25));
    imm_path_reset(&path);

    COND(imm_hmm_normalize_trans(hmm) == 0);
    imm_path_reset(&path);
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 1));
    CLOSE(imm_hmm_loglik(hmm, &AA, &path), imm_log(0.5) + 2 * imm_log(0.25));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state);
}

void test_hmm_loglik_two_states(void)
{
    struct imm_hmm *hmm = imm_hmm_new(&abc);

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_lprob_zero()};
    struct imm_normal_state *state0 = imm_normal_state_new(0, &abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.5), imm_log(0.25), imm_log(0.5),
                           imm_log(1.0)};
    struct imm_normal_state *state1 = imm_normal_state_new(1, &abc, lprobs1);

    imm_hmm_add_state(hmm, imm_super(state0));
    imm_hmm_set_start(hmm, imm_super(state0), imm_log(1.0));
    imm_hmm_add_state(hmm, imm_super(state1));

    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state0), imm_log(0.1));
    imm_hmm_set_trans(hmm, imm_super(state0), imm_super(state1), imm_log(0.2));
    imm_hmm_set_trans(hmm, imm_super(state1), imm_super(state1), imm_log(1.0));

    struct imm_path path = imm_path_init();
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state0)), 1));
    CLOSE(imm_hmm_loglik(hmm, &A, &path), -1.3862943611);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state0)), 1));
    COND(imm_lprob_is_zero(imm_hmm_loglik(hmm, &T, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state1)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &G, &path)));
    imm_path_reset(&path);

    COND(imm_hmm_normalize_trans(hmm) == 0);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state0)), 1));
    CLOSE(imm_hmm_loglik(hmm, &G, &path), -0.6931471806);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state0)), 1));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(state1)), 1));
    CLOSE(imm_hmm_loglik(hmm, &GT, &path), -1.0986122887);

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state0);
    imm_del(state1);
}

void test_hmm_loglik_mute_state(void)
{
    struct imm_hmm *hmm = imm_hmm_new(&abc);

    struct imm_mute_state *state = imm_mute_state_new(0, &abc);

    imm_hmm_add_state(hmm, imm_super(state));
    imm_hmm_set_start(hmm, imm_super(state), imm_log(1.0));

    imm_hmm_set_trans(hmm, imm_super(state), imm_super(state), imm_log(0.1));

    struct imm_path path = imm_path_init();
    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &A, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &T, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &G, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step_init(imm_id(imm_super(state)), 0));
    CLOSE(imm_hmm_loglik(hmm, &EMPTY, &path), 0.0);
    imm_path_reset(&path);

    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &GT, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &GT, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(state);
}

void test_hmm_loglik_two_mute_states(void)
{
    struct imm_hmm *hmm = imm_hmm_new(&abc);

    struct imm_mute_state *S0 = imm_mute_state_new(0, &abc);
    struct imm_mute_state *S1 = imm_mute_state_new(1, &abc);

    imm_hmm_add_state(hmm, imm_super(S0));
    imm_hmm_set_start(hmm, imm_super(S0), 0.0);
    imm_hmm_add_state(hmm, imm_super(S1));

    imm_hmm_set_trans(hmm, imm_super(S0), imm_super(S1), 0.0);

    struct imm_path path = imm_path_init();
    imm_path_add(&path, imm_step_init(imm_id(imm_super(S0)), 0));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(S1)), 0));
    CLOSE(imm_hmm_loglik(hmm, &EMPTY, &path), 0.0);

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(S0);
    imm_del(S1);
}

void test_hmm_loglik_invalid(void)
{
    struct imm_abc abc_ac = imm_abc_empty;
    imm_abc_init(&abc_ac, IMM_STR("AC"), '*');

    struct imm_seq C = imm_seq_empty;
    imm_seq_init(&C, IMM_STR("C"), &abc_ac);

    struct imm_hmm *hmm = imm_hmm_new(&abc_ac);

    struct imm_mute_state *S = imm_mute_state_new(0, &abc_ac);
    imm_hmm_add_state(hmm, imm_super(S));
    imm_hmm_set_start(hmm, imm_super(S), 0.0);

    struct imm_mute_state *M1 = imm_mute_state_new(1, &abc_ac);
    imm_hmm_add_state(hmm, imm_super(M1));

    imm_float const lprobs[] = {imm_log(0.8), imm_log(0.2)};
    struct imm_normal_state *M2 = imm_normal_state_new(2, &abc_ac, lprobs);
    imm_hmm_add_state(hmm, imm_super(M2));

    struct imm_mute_state *E = imm_mute_state_new(3, &abc_ac);
    imm_hmm_add_state(hmm, imm_super(E));

    imm_hmm_set_trans(hmm, imm_super(S), imm_super(M1), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M1), imm_super(M2), 0.0);
    imm_hmm_set_trans(hmm, imm_super(M2), imm_super(E), 0.0);
    imm_hmm_set_trans(hmm, imm_super(E), imm_super(E), 0.0);

    imm_hmm_normalize_trans(hmm);

    struct imm_path path = imm_path_init();
    imm_path_add(&path, imm_step_init(imm_id(imm_super(S)), 0));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(M2)), 1));
    imm_path_add(&path, imm_step_init(imm_id(imm_super(E)), 0));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(hmm, &C, &path)));

    imm_deinit(&path);
    imm_del(hmm);
    imm_del(S);
    imm_del(M1);
    imm_del(M2);
    imm_del(E);
}

void test_hmm_loglik_no_state(void)
{
    struct imm_hmm *hmm = imm_hmm_new(&abc);

    struct imm_path path = imm_path_init();
    COND(!imm_lprob_is_nan(imm_hmm_loglik(hmm, &EMPTY, &path)));

    imm_deinit(&path);
    imm_del(hmm);
}
