#include "imm_abc.h"
#include "imm_code.h"
#include "imm_hmm.h"
#include "imm_minctest.h"
#include "imm_mute_state.h"

static char *state_name(int id, char *name)
{
  name[0] = 'M';
  if (id == 0) name[1] = '0';
  else name[1] = '1';
  name[2] = '\0';
  return name;
}

static void hmm_dot(void)
{
  struct imm_abc abc;
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  struct imm_code code;
  imm_code_init(&code, &abc);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  eq(imm_hmm_add_state(hmm, &state0.super), 0);
  eq(imm_hmm_add_state(hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)), 0);

  FILE *fp = fopen("hmm.dot", "w");
  ok(fp);
  imm_hmm_dump(hmm, state_name, fp);
  fclose(fp);
  remove("hmm.dot");
  imm_hmm_del(hmm);
}

static void dp_dot(void)
{
  struct imm_abc abc;
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  struct imm_code code;
  imm_code_init(&code, &abc);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  eq(imm_hmm_add_state(hmm, &state0.super), 0);
  eq(imm_hmm_add_state(hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)), 0);

  eq(imm_hmm_set_start(hmm, &state0), 0);

  FILE *fp = fopen("dp.dot", "w");
  ok(fp);
  imm_hmm_dump(hmm, &state_name, fp);
  fclose(fp);
  remove("dp.dot");
  imm_hmm_del(hmm);
}

int main(void)
{
  lrun("hmm_dot", hmm_dot);
  lrun("dp_dot", dp_dot);
  return lfails != 0;
}
