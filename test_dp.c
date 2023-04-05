#include "abc.h"
#include "dp.h"
#include "ex1.h"
#include "ex2.h"
#include "hmm.h"
#include "lip/file/file.h"
#include "mute_state.h"
#include "normal_state.h"
#include "prod.h"
#include "task.h"
#include "test_helper.h"

static struct imm_abc abc;
static struct imm_code code;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq ATT;

static void dp_illegal(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), IMM_ENOEND);
  imm_dp_del(&dp);

  eq(imm_hmm_add_state(&hmm, &state.super), 0);
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), IMM_ENOSTART);
  imm_dp_del(&dp);

  eq(imm_hmm_set_start(&hmm, &state.super, log(0.1)), 0);
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), 0);
  imm_dp_del(&dp);

  eq(imm_hmm_set_trans(&hmm, &state.super, &state.super, log(0.5)), 0);
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), IMM_EMUTECYLES);
  imm_dp_del(&dp);
}

static void dp_empty_path(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  eq(imm_hmm_add_state(&hmm, &state.super), 0);
  eq(imm_hmm_set_start(&hmm, &state.super, log(0.1)), 0);
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), 0);

  struct imm_task *task = imm_task_new(&dp);
  imm_task_setup(task, &A);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 0);

  imm_task_del(task);
  imm_dp_del(&dp);
}

static void dp_one_mute(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  eq(imm_hmm_add_state(&hmm, &state.super), 0);

  eq(imm_hmm_set_start(&hmm, &state.super, log(0.3)), 0);
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &state.super, &dp), 0);

  struct imm_task *task = imm_task_new(&dp);
  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ENOSEQ);

  imm_task_setup(task, &EMPTY);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 1);

  imm_task_setup(task, &ATT);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 0);

  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_del(&dp);
}

static void dp_two_mutes(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 12, &abc);
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  eq(imm_hmm_add_state(&hmm, &state0.super), 0);
  eq(imm_hmm_add_state(&hmm, &state1.super), 0);

  eq(imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.5)), 0);

  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &state1.super, &dp), IMM_ENOSTART);

  eq(imm_hmm_set_start(&hmm, &state0.super, log(0.3)), 0);
  eq(imm_hmm_init_dp(&hmm, &state1.super, &dp), 0);

  struct imm_task *task = imm_task_new(&dp);

  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ENOSEQ);

  imm_task_setup(task, &EMPTY);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 2);
  eq(imm_path_step(&prod.path, 0)->seqlen, 0);
  eq(imm_path_step(&prod.path, 0)->state_id, 0);
  eq(imm_path_step(&prod.path, 1)->seqlen, 0);
  eq(imm_path_step(&prod.path, 1)->state_id, 12);

  imm_task_setup(task, &ATT);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 0);

  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
  imm_task_del(task);
}

static void dp_io_example1(void)
{
  struct lip_file file = {0};
  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  eq(imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp), 0);

  file.fp = fopen("dp_example1.imm", "wb");
  eq(imm_dp_pack(&dp, &file), 0);
  fclose(file.fp);
  imm_dp_del(&dp);

  imm_dp_init(&dp, &m->code);
  file.fp = fopen("dp_example1.imm", "rb");
  eq(imm_dp_unpack(&dp, &file), 0);
  fclose(file.fp);

  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq(imm_str("BMMMEJBMMME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -41.845375);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void dp_io_example2(void)
{
  struct lip_file file = {0};
  imm_ex2_init(10, (struct imm_span){1, 5});
  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(&m->hmm, &m->end.super, &dp);

  struct imm_abc const *abc = &m->dna->super.super;
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq(imm_str(imm_ex2_seq), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, 41.929977);
  imm_prod_cleanup(&prod);
  imm_task_del(task);

  file.fp = fopen("dp_frame.imm", "wb");
  eq(imm_dp_pack(&dp, &file), 0);
  fclose(file.fp);
  imm_dp_del(&dp);

  imm_dp_init(&dp, &m->code);
  file.fp = fopen("dp_frame.imm", "rb");
  eq(imm_dp_unpack(&dp, &file), 0);
  fclose(file.fp);

  task = imm_task_new(&dp);
  prod = imm_prod();
  seq = imm_seq(imm_str(imm_ex2_seq), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, 41.929977);

  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_del(&dp);
}

int main(void)
{
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  imm_code_init(&code, &abc);
  EMPTY = imm_seq(IMM_STR(""), &abc);
  A = imm_seq(IMM_STR("A"), &abc);
  ATT = imm_seq(IMM_STR("ATT"), &abc);

  lrun("dp_illegal", dp_illegal);
  lrun("dp_empty_path", dp_empty_path);
  lrun("dp_one_mute", dp_one_mute);
  lrun("dp_two_mutes", dp_two_mutes);
  lrun("dp_io_example1", dp_io_example1);
  lrun("dp_io_example2", dp_io_example2);

  return lfails != 0;
}