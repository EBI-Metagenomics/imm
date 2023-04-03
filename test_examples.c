#include "dp.h"
#include "ex1.h"
#include "ex2.h"
#include "ex3.h"
#include "hmm.h"
#include "mute_state.h"
#include "prod.h"
#include "task.h"
#include "test_helper.h"

static void example1_path(void)
{
  imm_ex1_init(10);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(imm_str("BMIMIMMMEJBMIIMIME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, true);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -49.892555);
  close(imm_hmm_loglik(&imm_ex1.hmm, &seq, &prod.path), -49.892555);
  eq(imm_path_nsteps(&prod.path), 32);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void example1_nopath(void)
{
  imm_ex1_init(10);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(imm_str("BMIMIMMMEJBMIIMIME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, false);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -49.892555);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void example1_removed_states_path(void)
{
  imm_ex1_init(3);
  imm_ex1_remove_insertion_states(3);
  imm_ex1_remove_deletion_states(3);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(imm_str("BMMMEJBMMME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, false);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -19.313255);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void example1_removed_states_nopath(void)
{
  imm_ex1_init(3);
  imm_ex1_remove_insertion_states(3);
  imm_ex1_remove_deletion_states(3);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(&imm_ex1.hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(imm_str("BMMMEJBMMME"), &m->abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, true);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -19.313255);
  close(imm_hmm_loglik(&imm_ex1.hmm, &seq, &prod.path), -19.313255);
  eq(imm_path_nsteps(&prod.path), 13);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void example2_15(void)
{
  imm_ex2_init(10, (struct imm_span){1, 5});

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(&m->hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq(imm_str(imm_ex2_seq), abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, true);

  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, 41.929977);

  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_del(&dp);
}

static void example2_24(void)
{
  imm_ex2_init(10, (struct imm_span){2, 4});

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(&m->hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq(imm_str(imm_ex2_seq), abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, true);

  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -84.087013);

  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_del(&dp);
}

static void example2_33(void)
{
  imm_ex2_init(10, (struct imm_span){3, 3});

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(&m->hmm, &m->end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq(imm_str(imm_ex2_seq), abc);
  eq(imm_task_setup(task, &seq), 0);
  imm_task_set_save_path(task, true);

  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  ok(imm_lprob_is_nan(prod.loglik));

  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_del(&dp);
}

static void msv10(void)
{
  imm_ex3_init(10);

  struct imm_ex3 *m = &imm_ex3;
  struct imm_dp dp;
  imm_hmm_init_dp(&imm_ex3.hmm, &m->T.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq(imm_str(imm_ex3_seq1), &m->abc);
  eq(imm_task_setup(task, &seq), 0);

  imm_task_set_save_path(task, true);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -7072.540039);

  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

int main(void)
{
  lrun("example1_path", example1_path);
  lrun("example1_nopath", example1_nopath);
  lrun("example1_removed_states_path", example1_removed_states_path);
  lrun("example1_removed_states_nopath", example1_removed_states_nopath);
  lrun("example2_15", example2_15);
  lrun("example2_24", example2_24);
  lrun("example2_33", example2_33);
  lrun("msv10", msv10);
  return lfails != 0;
}
