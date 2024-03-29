#include "abc.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state.h"
#include "vendor/minctest.h"

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
  close(imm_state_lprob(&state.super, &A), logf(0.25f));
  close(imm_state_lprob(&state.super, &C), logf(0.25f));
  close(imm_state_lprob(&state.super, &G), logf(0.5f));
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
  close(imm_state_lprob(&state.super, &EMPTY), 0.0);
  ok(imm_lprob_is_zero(imm_state_lprob(&state.super, &A)));
}

int main()
{
  lrun("normal_state", normal_state);
  lrun("mute_state", mute_state);
  return lfails != 0;
}
