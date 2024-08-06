#include "abc.h"
#include "hmm.h"
#include "minctest.h"
#include "mute_state.h"

static struct imm_abc abc = {0};
static struct imm_code code = {0};

static void hmm_state_id(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 0, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  eq(imm_hmm_add_state(hmm, &state.super), 0);
  eq(imm_hmm_add_state(hmm, &state.super), IMM_ESTATEPRESENT);

  imm_hmm_del(hmm);
}

static void hmm_set_trans(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  eq(imm_hmm_add_state(hmm, &state0.super), 0);
  eq(imm_hmm_add_state(hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)), 0);

  imm_hmm_del(hmm);
}

static void hmm_del_state(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  eq(imm_hmm_add_state(hmm, &state0.super), 0);
  eq(imm_hmm_add_state(hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)), 0);

  eq(imm_hmm_del_state(hmm, &state0.super), 0);
  eq(imm_hmm_del_state(hmm, &state1.super), 0);

  imm_hmm_del(hmm);
}

static void hmm_wrong_states(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 0, &abc);

  eq(imm_hmm_add_state(hmm, &state0.super), 0);

  eq(imm_hmm_set_start(hmm, &state1), IMM_ENOTFOUND);

  eq(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.3f)),
     IMM_ENOTFOUND);

  eq(imm_hmm_set_trans(hmm, &state1.super, &state0.super, logf(0.3f)),
     IMM_ENOTFOUND);

  eq(imm_hmm_normalize_state_trans(&state1.super), IMM_ENOTFOUND);

  eq(imm_hmm_normalize_state_trans(&state0.super), 0);

  imm_hmm_del(hmm);
}

int main(void)
{
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  imm_code_init(&code, &abc);
  lrun("hmm_state_id", hmm_state_id);
  lrun("hmm_set_trans", hmm_set_trans);
  lrun("hmm_del_state", hmm_del_state);
  lrun("hmm_wrong_states", hmm_wrong_states);
  return lfails != 0;
}
