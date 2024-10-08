#include "aye.h"
#include "imm_dp.h"
#include "imm_ex1.h"
#include "imm_ex2.h"
#include "imm_ex3.h"
#include "imm_hmm.h"
#include "imm_mute_state.h"
#include "imm_prod.h"
#include "imm_task.h"
#include "near.h"

static void example1_minimal(void)
{
  imm_ex1_init(3);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(imm_ex1.hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_seq seq = imm_seq_unsafe(imm_str("BMIMEJBMIME"), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -22.5321312f);
  near(imm_hmm_loglik(imm_ex1.hmm, &seq, &prod.path), -22.5321312f);
  aye(imm_path_nsteps(&prod.path) == 15);
  near(imm_path_score(&prod.path), -22.5321312f);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_ex1_cleanup();
}

static void example1_path(void)
{
  imm_ex1_init(10);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(imm_ex1.hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_seq seq = imm_seq_unsafe(imm_str("BMIMIMMMEJBMIIMIME"), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -49.892555f);
  near(imm_hmm_loglik(imm_ex1.hmm, &seq, &prod.path), -49.892555f);
  aye(imm_path_nsteps(&prod.path) == 32);
  near(imm_path_score(&prod.path), -49.892555f);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_ex1_cleanup();
}

static void example1_removed_states_path(void)
{
  imm_ex1_init(3);
  imm_ex1_remove_insertion_states(3);
  imm_ex1_remove_deletion_states(3);

  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  imm_hmm_init_dp(imm_ex1.hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_seq seq = imm_seq_unsafe(imm_str("BMMMEJBMMME"), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -19.313255f);
  near(imm_path_score(&prod.path), -19.313255f);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_ex1_cleanup();
}

static void example2_15(void)
{
  imm_ex2_init(10, imm_span(1, 5));

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(m->hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq_unsafe(imm_str(imm_ex2_seq), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);

  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, 41.929977f);
  near(imm_path_score(&prod.path), 41.929977f);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_ex2_cleanup();
}

static void example2_24(void)
{
  imm_ex2_init(10, imm_span(2, 4));

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(m->hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq_unsafe(imm_str(imm_ex2_seq), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);

  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -84.087013f);
  near(imm_path_score(&prod.path), -84.087013f);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_ex2_cleanup();
}

static void example2_33(void)
{
  imm_ex2_init(10, imm_span(3, 3));

  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(m->hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_nuclt const *nuclt = &m->dna->super;
  struct imm_abc const *abc = &nuclt->super;

  struct imm_seq seq = imm_seq_unsafe(imm_str(imm_ex2_seq), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);

  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_lprob_is_nan(prod.loglik));
  aye(imm_lprob_is_nan(imm_path_score(&prod.path)));

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_ex2_cleanup();
}

static void msv10(void)
{
  imm_ex3_init(10);

  struct imm_ex3 *m = &imm_ex3;
  struct imm_dp dp;
  imm_hmm_init_dp(imm_ex3.hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_seq seq = imm_seq_unsafe(imm_str(imm_ex3_seq1), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);

  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -7072.540039f);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_ex3_cleanup();
}

int main(void)
{
  aye_begin();
  example1_minimal();
  example1_path();
  example1_removed_states_path();
  example2_15();
  example2_24();
  example2_33();
  msv10();
  return aye_end();
}
