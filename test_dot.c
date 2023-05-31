#include "abc.h"
#include "code.h"
#include "hmm.h"
#include "mute_state.h"
#include "test_helper.h"

static unsigned state_name(unsigned id, char *name)
{
  name[0] = 'M';
  if (id == 0) name[1] = '0';
  else name[1] = '1';
  name[2] = '\0';
  return 2;
}

static void hmm_dot(void)
{
  struct imm_abc abc;
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  struct imm_code code;
  imm_code_init(&code, &abc);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  eq(imm_hmm_add_state(&hmm, &state0.super), 0);
  eq(imm_hmm_add_state(&hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.5)), 0);

  FILE *fd = fopen("hmm.dot", "w");
  ok(fd);
  imm_hmm_write_dot(&hmm, fd, state_name);
  fclose(fd);
  remove("hmm.dot");
}

static void dp_dot(void)
{
  struct imm_abc abc;
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  struct imm_code code;
  imm_code_init(&code, &abc);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  eq(imm_hmm_add_state(&hmm, &state0.super), 0);
  eq(imm_hmm_add_state(&hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.5)), 0);

  eq(imm_hmm_set_start(&hmm, &state0.super, 0.5f), 0);

  FILE *fd = fopen("dp.dot", "w");
  ok(fd);
  imm_hmm_write_dot(&hmm, fd, &state_name);
  fclose(fd);
  remove("dp.dot");
}

int main(void)
{
  lrun("hmm_dot", hmm_dot);
  lrun("dp_dot", dp_dot);
  return lfails != 0;
}
