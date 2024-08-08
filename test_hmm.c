#include "aye.h"
#include "imm_abc.h"
#include "imm_hmm.h"
#include "imm_mute_state.h"
#include <math.h>

static struct imm_abc abc = {0};
static struct imm_code code = {0};

static void hmm_state_id(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 0, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  aye(imm_hmm_add_state(hmm, &state.super) == 0);
  aye(imm_hmm_add_state(hmm, &state.super) == IMM_ESTATEPRESENT);

  imm_hmm_del(hmm);
}

static void hmm_set_trans(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  aye(imm_hmm_add_state(hmm, &state0.super) == 0);
  aye(imm_hmm_add_state(hmm, &state1.super) == 0);

  aye(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)) == 0);

  imm_hmm_del(hmm);
}

static void hmm_del_state(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  aye(imm_hmm_add_state(hmm, &state0.super) == 0);
  aye(imm_hmm_add_state(hmm, &state1.super) == 0);

  aye(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)) == 0);

  aye(imm_hmm_del_state(hmm, &state0.super) == 0);
  aye(imm_hmm_del_state(hmm, &state1.super) == 0);

  imm_hmm_del(hmm);
}

static void hmm_wrong_states(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 0, &abc);

  aye(imm_hmm_add_state(hmm, &state0.super) == 0);

  aye(imm_hmm_set_start(hmm, &state1) == IMM_ENOTFOUND);

  aye(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.3f)) ==
      IMM_ENOTFOUND);

  aye(imm_hmm_set_trans(hmm, &state1.super, &state0.super, logf(0.3f)) ==
      IMM_ENOTFOUND);

  aye(imm_hmm_normalize_state_trans(&state1.super) == IMM_ENOTFOUND);

  aye(imm_hmm_normalize_state_trans(&state0.super) == 0);

  imm_hmm_del(hmm);
}

int main(void)
{
  aye_begin();
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  imm_code_init(&code, &abc);
  hmm_state_id();
  hmm_set_trans();
  hmm_del_state();
  hmm_wrong_states();
  return aye_end();
}
