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
#include "minctest.h"

static char *state_name(int id, char *name)
{
  char *x = name;
  if (id == 0) *(x++) = 'B';
  if (id == 1) *(x++) = 'X';
  if (id == 2) *(x++) = 'E';
  if (id == 3) *(x++) = 'J';

  *x = '\0';
  return name;
}

#define ASSERT_SEQ(PATH, IDX, STATE_ID, SEQSIZE, SCORE)                        \
  do                                                                           \
  {                                                                            \
    eq(imm_path_step(&PATH, IDX)->state_id, STATE_ID);                         \
    eq(imm_path_step(&PATH, IDX)->seqsize, SEQSIZE);                           \
    near(imm_path_step(&PATH, IDX)->score, SCORE);                            \
  } while (0);

#define B 0
#define X 1
#define E 2
#define J 3
#define START 4
#define END 5

static void odd1(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, imm_str("XJ"), '*'), 0);

  struct imm_mute_state b = {0};
  imm_mute_state_init(&b, B, &abc);

  struct imm_normal_state x = {0};
  imm_normal_state_init(&x, X, &abc, (float[]){1., imm_lprob_zero()});

  struct imm_mute_state e = {0};
  imm_mute_state_init(&e, E, &abc);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, START, &abc);

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, END, &abc);

  struct imm_code code = {0};
  imm_code_init(&code, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  eq(imm_hmm_add_state(hmm, &b.super), 0);
  eq(imm_hmm_add_state(hmm, &x.super), 0);
  eq(imm_hmm_add_state(hmm, &e.super), 0);
  eq(imm_hmm_add_state(hmm, &start.super), 0);
  eq(imm_hmm_add_state(hmm, &end.super), 0);

  eq(imm_hmm_set_start(hmm, &b), 0);
  eq(imm_hmm_set_trans(hmm, &b.super, &x.super, 100.), 0);
  eq(imm_hmm_set_trans(hmm, &x.super, &e.super, 1000.), 0);
  eq(imm_hmm_set_trans(hmm, &e.super, &b.super, 10000.), 0);
  eq(imm_hmm_set_end(hmm, &e), 0);
  eq(imm_hmm_set_trans(hmm, &start.super, &b.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &e.super, &end.super, 0.), 0);

  eq(imm_hmm_set_start(hmm, &start), 0);
  eq(imm_hmm_set_end(hmm, &end), 0);

  struct imm_dp dp = {0};
  eq(imm_hmm_init_dp(hmm, &dp), 0);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq_unsafe(imm_str("XX"), &abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 8);
  near(imm_hmm_loglik(hmm, &seq, &prod.path), 12202.);
  near(prod.loglik, 12202.);
  imm_dp_set_state_name(&dp, &state_name);

  ASSERT_SEQ(prod.path, 0, START, 0, 0);
  ASSERT_SEQ(prod.path, 1, B, 0, 0);
  ASSERT_SEQ(prod.path, 2, X, 1, 100);
  ASSERT_SEQ(prod.path, 3, E, 0, 1101);
  ASSERT_SEQ(prod.path, 4, B, 0, 11101);
  ASSERT_SEQ(prod.path, 5, X, 1, 11201);
  ASSERT_SEQ(prod.path, 6, E, 0, 12202);
  ASSERT_SEQ(prod.path, 7, END, 0, 12202);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
  imm_hmm_del(hmm);
}

static void odd2(void)
{
  struct imm_abc abc = {0};
  eq(imm_abc_init(&abc, imm_str("XJ"), '*'), 0);

  struct imm_mute_state b = {0};
  imm_mute_state_init(&b, B, &abc);

  struct imm_normal_state x = {0};
  imm_normal_state_init(&x, X, &abc, (float[]){0., imm_lprob_zero()});

  struct imm_mute_state e = {0};
  imm_mute_state_init(&e, E, &abc);

  struct imm_normal_state j = {0};
  imm_normal_state_init(&j, J, &abc, (float[]){imm_lprob_zero(), 0.});

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, START, &abc);

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, END, &abc);

  struct imm_code code = {0};
  imm_code_init(&code, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  eq(imm_hmm_add_state(hmm, &b.super), 0);
  eq(imm_hmm_add_state(hmm, &x.super), 0);
  eq(imm_hmm_add_state(hmm, &e.super), 0);
  eq(imm_hmm_add_state(hmm, &j.super), 0);
  eq(imm_hmm_add_state(hmm, &start.super), 0);
  eq(imm_hmm_add_state(hmm, &end.super), 0);

  eq(imm_hmm_set_trans(hmm, &b.super, &x.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &x.super, &e.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &e.super, &b.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &e.super, &j.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &j.super, &j.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &j.super, &b.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &start.super, &b.super, 0.), 0);
  eq(imm_hmm_set_trans(hmm, &e.super, &end.super, 0.), 0);

  eq(imm_hmm_set_start(hmm, &start), 0);
  eq(imm_hmm_set_end(hmm, &end), 0);

  struct imm_dp dp = {0};
  eq(imm_hmm_init_dp(hmm, &dp), 0);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  struct imm_seq seq = imm_seq_unsafe(imm_str("XJX"), &abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 9);
  near(imm_path_score(&prod.path), 0);
  near(imm_hmm_loglik(hmm, &seq, &prod.path), 0);
  near(prod.loglik, 0);
  imm_dp_set_state_name(&dp, &state_name);

  ASSERT_SEQ(prod.path, 0, START, 0, 0);
  ASSERT_SEQ(prod.path, 1, B, 0, 0);
  ASSERT_SEQ(prod.path, 2, X, 1, 0);
  ASSERT_SEQ(prod.path, 3, E, 0, 0);
  ASSERT_SEQ(prod.path, 4, J, 1, 0);
  ASSERT_SEQ(prod.path, 5, B, 0, 0);
  ASSERT_SEQ(prod.path, 6, X, 1, 0);
  ASSERT_SEQ(prod.path, 7, E, 0, 0);
  ASSERT_SEQ(prod.path, 8, END, 0, 0);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
  imm_hmm_del(hmm);
}

int main(void)
{
  lrun("odd1", odd1);
  lrun("odd2", odd2);
  return lfails != 0;
}
