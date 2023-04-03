#include "abc.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "state.h"
#include "test_helper.h"

static void normal_state(void)
{
  struct imm_abc abc = {0};
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  struct imm_seq A = imm_seq(IMM_STR("A"), &abc);
  struct imm_seq C = imm_seq(IMM_STR("C"), &abc);
  struct imm_seq G = imm_seq(IMM_STR("G"), &abc);
  struct imm_seq T = imm_seq(IMM_STR("T"), &abc);

  float const lprobs[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
  struct imm_normal_state state;
  imm_normal_state_init(&state, 0, &abc, lprobs);

  eq(imm_state_id(&state.super), 0);
  close(imm_state_lprob(&state.super, &A), log(0.25));
  close(imm_state_lprob(&state.super, &C), log(0.25));
  close(imm_state_lprob(&state.super, &G), log(0.5));
  ok(imm_lprob_is_zero(imm_state_lprob(&state.super, &T)));
}

static void mute_state(void)
{
  struct imm_abc abc = {0};
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  struct imm_seq EMPTY = imm_seq(IMM_STR(""), &abc);
  struct imm_seq A = imm_seq(IMM_STR("A"), &abc);

  struct imm_mute_state state = {0};
  imm_mute_state_init(&state, 43, &abc);

  eq(imm_state_id(&state.super), 43);
  close(imm_state_lprob(&state.super, &EMPTY), 0.0);
  ok(imm_lprob_is_zero(imm_state_lprob(&state.super, &A)));
}

int main()
{
  lrun("normal_state", normal_state);
  lrun("mute_state", mute_state);
  return lfails != 0;
}
