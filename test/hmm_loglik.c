#include "hope.h"
#include "imm/imm.h"

void test_hmm_loglik_single_state(void);
void test_hmm_loglik_two_states(void);
void test_hmm_loglik_mute_state(void);
void test_hmm_loglik_two_mute_states(void);
void test_hmm_loglik_invalid(void);
void test_hmm_loglik_no_state(void);

static struct imm_abc abc;
static struct imm_code code;
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
    imm_code_init(&code, &abc);
    EMPTY = imm_seq(IMM_STR(""), &abc);
    A = imm_seq(IMM_STR("A"), &abc);
    T = imm_seq(IMM_STR("T"), &abc);
    G = imm_seq(IMM_STR("G"), &abc);
    AG = imm_seq(IMM_STR("AG"), &abc);
    AA = imm_seq(IMM_STR("AA"), &abc);
    GT = imm_seq(IMM_STR("GT"), &abc);

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

    struct imm_normal_state state;
    imm_normal_state_init(&state, 0, &abc, lprobs);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &code);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state)), IMM_OK);
    EQ(imm_hmm_set_start(&hmm, imm_super(&state), imm_log(0.5)), IMM_OK);
    EQ(imm_hmm_normalize_trans(&hmm), IMM_OK);

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &A, &path), imm_log(0.5) + imm_log(0.25));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &A, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &EMPTY, &path)));
    imm_path_reset(&path);

    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &A, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &AG, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &AA, &path)));
    imm_path_reset(&path);

    EQ(imm_hmm_normalize_trans(&hmm), 0);
    EQ(imm_hmm_set_trans(&hmm, imm_super(&state), imm_super(&state),
                         imm_lprob_zero()),
       IMM_NON_FINITE_PROBABILITY);
    EQ(imm_hmm_normalize_trans(&hmm), 0);
    EQ(imm_hmm_set_trans(&hmm, imm_super(&state), imm_super(&state),
                         imm_log(0.5)),
       IMM_OK);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &AA, &path),
          2 * imm_log(0.5) + 2 * imm_log(0.25));
    imm_path_reset(&path);

    COND(imm_hmm_normalize_trans(&hmm) == 0);
    imm_path_reset(&path);
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &AA, &path), imm_log(0.5) + 2 * imm_log(0.25));

    imm_del(&path);
}

void test_hmm_loglik_two_states(void)
{
    struct imm_hmm hmm;
    imm_code_init(&code, &abc);
    imm_hmm_init(&hmm, &code);

    imm_float lprobs0[] = {imm_log(0.25), imm_log(0.25), imm_log(0.5),
                           imm_lprob_zero()};
    struct imm_normal_state state0;
    imm_normal_state_init(&state0, 0, &abc, lprobs0);

    imm_float lprobs1[] = {imm_log(0.5), imm_log(0.25), imm_log(0.5),
                           imm_log(1.0)};
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

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state0)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &A, &path), -1.3862943611);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state0)), 1));
    COND(imm_lprob_is_zero(imm_hmm_loglik(&hmm, &T, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state1)), 1));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &G, &path)));
    imm_path_reset(&path);

    COND(imm_hmm_normalize_trans(&hmm) == 0);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state0)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &G, &path), -0.6931471806);
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state0)), 1));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state1)), 1));
    CLOSE(imm_hmm_loglik(&hmm, &GT, &path), -1.0986122887);

    imm_del(&path);
}

void test_hmm_loglik_mute_state(void)
{
    struct imm_hmm hmm;
    imm_code_init(&code, &abc);
    imm_hmm_init(&hmm, &code);

    struct imm_mute_state state;
    imm_mute_state_init(&state, 0, &abc);

    imm_hmm_add_state(&hmm, imm_super(&state));
    imm_hmm_set_start(&hmm, imm_super(&state), imm_log(1.0));

    imm_hmm_set_trans(&hmm, imm_super(&state), imm_super(&state), imm_log(0.1));

    struct imm_path path = imm_path();
    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &A, &path)));
    imm_path_reset(&path);

    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &T, &path)));
    imm_path_reset(&path);

    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &G, &path)));
    imm_path_reset(&path);

    imm_path_add(&path, imm_step(imm_state_id(imm_super(&state)), 0));
    CLOSE(imm_hmm_loglik(&hmm, &EMPTY, &path), 0.0);
    imm_path_reset(&path);

    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &GT, &path)));
    imm_path_reset(&path);

    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &GT, &path)));

    imm_del(&path);
}

void test_hmm_loglik_two_mute_states(void)
{
    struct imm_hmm hmm;
    imm_code_init(&code, &abc);
    imm_hmm_init(&hmm, &code);

    struct imm_mute_state S0;
    imm_mute_state_init(&S0, 0, &abc);
    struct imm_mute_state S1;
    imm_mute_state_init(&S1, 1, &abc);

    imm_hmm_add_state(&hmm, imm_super(&S0));
    imm_hmm_set_start(&hmm, imm_super(&S0), 0.0);
    imm_hmm_add_state(&hmm, imm_super(&S1));

    imm_hmm_set_trans(&hmm, imm_super(&S0), imm_super(&S1), 0.0);

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&S0)), 0));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&S1)), 0));
    CLOSE(imm_hmm_loglik(&hmm, &EMPTY, &path), 0.0);

    imm_del(&path);
}

void test_hmm_loglik_invalid(void)
{
    struct imm_abc abc_ac = imm_abc_empty;
    imm_abc_init(&abc_ac, IMM_STR("AC"), '*');

    struct imm_seq C = imm_seq(IMM_STR("C"), &abc_ac);

    struct imm_hmm hmm;
    imm_code_init(&code, &abc_ac);
    imm_hmm_init(&hmm, &code);

    struct imm_mute_state S;
    imm_mute_state_init(&S, 0, &abc_ac);
    imm_hmm_add_state(&hmm, imm_super(&S));
    imm_hmm_set_start(&hmm, imm_super(&S), 0.0);

    struct imm_mute_state M1;
    imm_mute_state_init(&M1, 1, &abc_ac);
    imm_hmm_add_state(&hmm, imm_super(&M1));

    imm_float const lprobs[] = {imm_log(0.8), imm_log(0.2)};
    struct imm_normal_state M2;
    imm_normal_state_init(&M2, 2, &abc_ac, lprobs);
    imm_hmm_add_state(&hmm, imm_super(&M2));

    struct imm_mute_state E;
    imm_mute_state_init(&E, 3, &abc_ac);
    imm_hmm_add_state(&hmm, imm_super(&E));

    imm_hmm_set_trans(&hmm, imm_super(&S), imm_super(&M1), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M1), imm_super(&M2), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&M2), imm_super(&E), 0.0);
    imm_hmm_set_trans(&hmm, imm_super(&E), imm_super(&E), 0.0);

    imm_hmm_normalize_trans(&hmm);

    struct imm_path path = imm_path();
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&S)), 0));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&M2)), 1));
    imm_path_add(&path, imm_step(imm_state_id(imm_super(&E)), 0));
    COND(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &C, &path)));

    imm_del(&path);
}

void test_hmm_loglik_no_state(void)
{
    struct imm_hmm hmm;
    imm_code_init(&code, &abc);
    imm_hmm_init(&hmm, &code);

    struct imm_path path = imm_path();
    COND(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &EMPTY, &path)));

    imm_del(&path);
}
