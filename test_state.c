#include "imm_abc.h"
#include "imm_lprob.h"
#include "imm_minctest.h"
#include "imm_mute_state.h"
#include "imm_normal_state.h"
#include "imm_state.h"

static void normal_state(void)
{
  struct imm_abc abc = {0};
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  struct imm_seq A = imm_seq_unsafe(imm_str("A"), &abc);
  struct imm_seq C = imm_seq_unsafe(imm_str("C"), &abc);
  struct imm_seq G = imm_seq_unsafe(imm_str("G"), &abc);
  struct imm_seq T = imm_seq_unsafe(imm_str("T"), &abc);

  float const lprobs[] = {logf(0.25f), logf(0.25f), logf(0.5f),
                          imm_lprob_zero()};
  struct imm_normal_state state;
  imm_normal_state_init(&state, 0, &abc, lprobs);

  eq(imm_state_id(&state.super), 0U);
  near(imm_state_lprob(&state.super, &A), logf(0.25f));
  near(imm_state_lprob(&state.super, &C), logf(0.25f));
  near(imm_state_lprob(&state.super, &G), logf(0.5f));
  ok(imm_lprob_is_zero(imm_state_lprob(&state.super, &T)));
}

static void mute_state(void)
{
  struct imm_abc abc = {0};
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  struct imm_seq EMPTY = imm_seq_unsafe(imm_str(""), &abc);
  struct imm_seq A = imm_seq_unsafe(imm_str("A"), &abc);

  struct imm_mute_state state = {0};
  imm_mute_state_init(&state, 43, &abc);

  eq(imm_state_id(&state.super), 43U);
  near(imm_state_lprob(&state.super, &EMPTY), 0.0);
  ok(imm_lprob_is_zero(imm_state_lprob(&state.super, &A)));
}

int main(void)
{
  lrun("normal_state", normal_state);
  lrun("mute_state", mute_state);
  return lfails != 0;
}
