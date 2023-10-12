#include "abc.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "path.h"
#include "vendor/minctest.h"

static struct imm_abc abc = {0};
static struct imm_code code = {0};
static struct imm_seq EMPTY = {0};
static struct imm_seq A = {0};
static struct imm_seq T = {0};
static struct imm_seq G = {0};
static struct imm_seq AG = {0};
static struct imm_seq AA = {0};
static struct imm_seq GT = {0};

static void hmm_loglik_single_state(void)
{
  float lprobs[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};

  struct imm_normal_state state;
  imm_normal_state_init(&state, 0, &abc, lprobs);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &abc);

  eq(imm_hmm_add_state(&hmm, &start.super), 0);
  eq(imm_hmm_add_state(&hmm, &state.super), 0);
  eq(imm_hmm_set_trans(&hmm, &start.super, &state.super, 0), 0);
  eq(imm_hmm_set_start(&hmm, &start), 0);
  eq(imm_hmm_normalize_trans(&hmm), 0);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &A, &path), log(1.0) + log(0.25));
  imm_path_reset(&path);

  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &A, &path)));
  imm_path_reset(&path);

  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &EMPTY, &path)));
  imm_path_reset(&path);

  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &A, &path)));
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &AG, &path)));
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &AA, &path)));
  imm_path_reset(&path);

  eq(imm_hmm_normalize_trans(&hmm), 0);
  eq(imm_hmm_set_trans(&hmm, &state.super, &state.super, imm_lprob_zero()),
     IMM_EINVAL);
  eq(imm_hmm_normalize_trans(&hmm), 0);
  eq(imm_hmm_set_trans(&hmm, &state.super, &state.super, log(0.5)), 0);
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &AA, &path), log(0.25) + 3 * log(0.5));
  imm_path_reset(&path);

  ok(imm_hmm_normalize_trans(&hmm) == 0);
  imm_path_reset(&path);
  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &AA, &path), log(1.0) + 2 * log(0.25));

  imm_path_cleanup(&path);
}

static void hmm_loglik_two_states(void)
{
  struct imm_hmm hmm;
  imm_code_init(&code, &abc);
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 2, &abc);

  float lprobs0[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
  struct imm_normal_state state0;
  imm_normal_state_init(&state0, 0, &abc, lprobs0);

  float lprobs1[] = {log(0.5), log(0.25), log(0.5), log(1.0)};
  struct imm_normal_state state1;
  imm_normal_state_init(&state1, 1, &abc, lprobs1);

  eq(imm_hmm_add_state(&hmm, &start.super), 0);
  eq(imm_hmm_add_state(&hmm, &state0.super), 0);
  eq(imm_hmm_set_start(&hmm, &start), 0);
  eq(imm_hmm_set_trans(&hmm, &start.super, &state0.super, 0), 0);
  eq(imm_hmm_add_state(&hmm, &state1.super), 0);

  imm_hmm_set_trans(&hmm, &state0.super, &state0.super, log(0.1));
  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.2));
  imm_hmm_set_trans(&hmm, &state1.super, &state1.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state0.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &A, &path), -1.3862943611);
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state0.super), 1, 0));
  ok(imm_lprob_is_zero(imm_hmm_loglik(&hmm, &T, &path)));
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state1.super), 1, 0));
  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &G, &path)));
  imm_path_reset(&path);

  ok(imm_hmm_normalize_trans(&hmm) == 0);

  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state0.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &G, &path), -0.6931471806);
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&start.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state0.super), 1, 0));
  imm_path_add(&path, imm_step(imm_state_id(&state1.super), 1, 0));
  close(imm_hmm_loglik(&hmm, &GT, &path), -1.0986122887);

  imm_path_cleanup(&path);
}

static void hmm_loglik_mute_state(void)
{
  struct imm_hmm hmm;
  imm_code_init(&code, &abc);
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state state;
  imm_mute_state_init(&state, 0, &abc);

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state);

  imm_hmm_set_trans(&hmm, &state.super, &state.super, log(0.1));

  struct imm_path path = imm_path();
  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &A, &path)));
  imm_path_reset(&path);

  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &T, &path)));
  imm_path_reset(&path);

  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &G, &path)));
  imm_path_reset(&path);

  imm_path_add(&path, imm_step(imm_state_id(&state.super), 0, 0));
  close(imm_hmm_loglik(&hmm, &EMPTY, &path), 0.0);
  imm_path_reset(&path);

  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &GT, &path)));
  imm_path_reset(&path);

  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &GT, &path)));

  imm_path_cleanup(&path);
}

static void hmm_loglik_two_mute_states(void)
{
  struct imm_hmm hmm;
  imm_code_init(&code, &abc);
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state S0;
  imm_mute_state_init(&S0, 0, &abc);
  struct imm_mute_state S1;
  imm_mute_state_init(&S1, 1, &abc);

  imm_hmm_add_state(&hmm, &S0.super);
  imm_hmm_set_start(&hmm, &S0);
  imm_hmm_add_state(&hmm, &S1.super);

  imm_hmm_set_trans(&hmm, &S0.super, &S1.super, 0.0);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(imm_state_id(&S0.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&S1.super), 0, 0));
  close(imm_hmm_loglik(&hmm, &EMPTY, &path), 0.0);

  imm_path_cleanup(&path);
}

static void hmm_loglik_invalid(void)
{
  struct imm_abc abc_ac = {0};
  imm_abc_init(&abc_ac, imm_str("AC"), '*');

  struct imm_seq C = imm_seq(imm_str("C"), &abc_ac);

  struct imm_hmm hmm;
  imm_code_init(&code, &abc_ac);
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state S;
  imm_mute_state_init(&S, 0, &abc_ac);
  imm_hmm_add_state(&hmm, &S.super);
  imm_hmm_set_start(&hmm, &S);

  struct imm_mute_state M1;
  imm_mute_state_init(&M1, 1, &abc_ac);
  imm_hmm_add_state(&hmm, &M1.super);

  float const lprobs[] = {log(0.8), log(0.2)};
  struct imm_normal_state M2;
  imm_normal_state_init(&M2, 2, &abc_ac, lprobs);
  imm_hmm_add_state(&hmm, &M2.super);

  struct imm_mute_state E;
  imm_mute_state_init(&E, 3, &abc_ac);
  imm_hmm_add_state(&hmm, &E.super);

  imm_hmm_set_trans(&hmm, &S.super, &M1.super, 0.0);
  imm_hmm_set_trans(&hmm, &M1.super, &M2.super, 0.0);
  imm_hmm_set_trans(&hmm, &M2.super, &E.super, 0.0);
  imm_hmm_set_trans(&hmm, &E.super, &E.super, 0.0);

  imm_hmm_normalize_trans(&hmm);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(imm_state_id(&S.super), 0, 0));
  imm_path_add(&path, imm_step(imm_state_id(&M2.super), 1, 0));
  imm_path_add(&path, imm_step(imm_state_id(&E.super), 0, 0));
  ok(!imm_lprob_is_finite(imm_hmm_loglik(&hmm, &C, &path)));

  imm_path_cleanup(&path);
}

static void hmm_loglik_no_state(void)
{
  struct imm_hmm hmm;
  imm_code_init(&code, &abc);
  imm_hmm_init(&hmm, &code);

  struct imm_path path = imm_path();
  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &EMPTY, &path)));

  imm_path_cleanup(&path);
}

int main(void)
{
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  imm_code_init(&code, &abc);
  EMPTY = imm_seq(imm_str(""), &abc);
  A = imm_seq(imm_str("A"), &abc);
  T = imm_seq(imm_str("T"), &abc);
  G = imm_seq(imm_str("G"), &abc);
  AG = imm_seq(imm_str("AG"), &abc);
  AA = imm_seq(imm_str("AA"), &abc);
  GT = imm_seq(imm_str("GT"), &abc);
  lrun("hmm_loglik_single_state", hmm_loglik_single_state);
  lrun("hmm_loglik_two_states", hmm_loglik_two_states);
  lrun("hmm_loglik_mute_state", hmm_loglik_mute_state);
  lrun("hmm_loglik_two_mute_states", hmm_loglik_two_mute_states);
  lrun("hmm_loglik_invalid", hmm_loglik_invalid);
  lrun("hmm_loglik_no_state", hmm_loglik_no_state);
  return lfails != 0;
}
