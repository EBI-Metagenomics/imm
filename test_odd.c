#include "abc.h"
#include "code.h"
#include "dp.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "path.h"
#include "prod.h"
#include "seq.h"
#include "task.h"
#include "vendor/minctest.h"

static char *state_name(unsigned id, char *name)
{
  char *x = name;
  if (id == 0) *(x++) = 'B';
  if (id == 1) *(x++) = 'X';
  if (id == 2) *(x++) = 'E';
  if (id == 3) *(x++) = 'J';

  *x = '\0';
  return name;
}

static void odd1(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("XJ"), '*'), 0);

  struct imm_mute_state B = {0};
  imm_mute_state_init(&B, 0, &abc);

  struct imm_normal_state X = {0};
  imm_normal_state_init(&X, 1, &abc, (float[]){1., imm_lprob_zero()});

  struct imm_mute_state E = {0};
  imm_mute_state_init(&E, 2, &abc);

  struct imm_code code = {0};
  imm_code_init(&code, &abc);
  struct imm_hmm hmm = {0};
  imm_hmm_init(&hmm, &code);

  imm_hmm_add_state(&hmm, &B.super);
  imm_hmm_add_state(&hmm, &X.super);
  imm_hmm_add_state(&hmm, &E.super);

  imm_hmm_set_start(&hmm, &B.super, 10.0);
  imm_hmm_set_trans(&hmm, &B.super, &X.super, 100.);
  imm_hmm_set_trans(&hmm, &X.super, &E.super, 1000.);
  imm_hmm_set_trans(&hmm, &E.super, &B.super, 10000.);

  struct imm_dp dp = {0};
  imm_hmm_init_dp(&hmm, &E.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(IMM_STR("XX"), &abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 6U);
  close(imm_hmm_loglik(&hmm, &seq, &prod.path), 12212.);
  close(prod.loglik, 12212.);
  imm_dp_dump_path(&dp, task, &prod, &state_name);
  imm_hmm_write_dot(&hmm, stdout, &state_name);
  imm_dp_write_dot(&dp, stdout, &state_name);

  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
  imm_task_del(task);
}

static void odd2(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, IMM_STR("XJ"), '*'), 0);

  struct imm_mute_state B = {0};
  imm_mute_state_init(&B, 0, &abc);

  struct imm_normal_state X = {0};
  imm_normal_state_init(&X, 1, &abc, (float[]){0., imm_lprob_zero()});

  struct imm_mute_state E = {0};
  imm_mute_state_init(&E, 2, &abc);

  struct imm_normal_state J = {0};
  imm_normal_state_init(&J, 3, &abc, (float[]){imm_lprob_zero(), 0.});

  struct imm_code code = {0};
  imm_code_init(&code, &abc);
  struct imm_hmm hmm = {0};
  imm_hmm_init(&hmm, &code);

  imm_hmm_add_state(&hmm, &B.super);
  imm_hmm_add_state(&hmm, &X.super);
  imm_hmm_add_state(&hmm, &E.super);
  imm_hmm_add_state(&hmm, &J.super);

  imm_hmm_set_start(&hmm, &B.super, 0.);
  imm_hmm_set_trans(&hmm, &B.super, &X.super, 0.);
  imm_hmm_set_trans(&hmm, &X.super, &E.super, 0.);
  imm_hmm_set_trans(&hmm, &E.super, &B.super, 0.);
  imm_hmm_set_trans(&hmm, &E.super, &J.super, 0.);
  imm_hmm_set_trans(&hmm, &J.super, &J.super, 0.);
  imm_hmm_set_trans(&hmm, &J.super, &B.super, 0.);

  struct imm_dp dp = {0};
  imm_hmm_init_dp(&hmm, &E.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(IMM_STR("XJX"), &abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 7U);
  close(imm_hmm_loglik(&hmm, &seq, &prod.path), 0);
  close(prod.loglik, 0);
  imm_dp_dump_path(&dp, task, &prod, &state_name);
  imm_hmm_write_dot(&hmm, stdout, &state_name);
  imm_dp_write_dot(&dp, stdout, &state_name);

  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
  imm_task_del(task);
}

int main(void)
{
  lrun("odd1", odd1);
  lrun("odd2", odd2);
  return lfails != 0;
}
