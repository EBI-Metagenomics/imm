#include "abc.h"
#include "code.h"
#include "dna.h"
#include "dp.h"
#include "eseq.h"
#include "frame_state.h"
#include "hmm.h"
#include "mute_state.h"
#include "normal_state.h"
#include "prod.h"
#include "table_state.h"
#include "task.h"
#include "vendor/minctest.h"

#define S 0
#define N 1
#define T 2
#define E 3

static struct imm_abc const *abc = &imm_dna_iupac.super.super;
static struct imm_code code = {0};
static struct imm_hmm *hmm = NULL;
static struct imm_eseq eseq = {0};
static struct imm_prod prod = {0};

static struct imm_mute_state start = {0};
static struct imm_normal_state normal = {0};
static struct imm_table_state table = {0};
static struct imm_mute_state end = {0};

static void setup(void)
{
  imm_code_init(&code, abc);
  hmm = imm_hmm_new(&code);
  imm_eseq_init(&eseq, &code);
  prod = imm_prod();
}

static void cleanup(void)
{
  imm_prod_cleanup(&prod);
  imm_eseq_cleanup(&eseq);
  imm_hmm_del(hmm);
}

static char *state_name(int id, char *name)
{
  if (id == S) strcpy(name, "S");
  else if (id == N) strcpy(name, "N");
  else if (id == T) strcpy(name, "T");
  else if (id == E) strcpy(name, "E");
  else strcpy(name, "?");
  return name;
}

#define ASSERT_SEQ(PATH, IDX, STATE_ID, SEQSIZE, SCORE)                        \
  do                                                                           \
  {                                                                            \
    eq(imm_path_step(&PATH, IDX)->state_id, STATE_ID);                         \
    eq(imm_path_step(&PATH, IDX)->seqsize, SEQSIZE);                           \
    close(imm_path_step(&PATH, IDX)->score, SCORE);                            \
  } while (0);

static void states_sne(void)
{
  setup();

  imm_mute_state_init(&start, S, abc);
  imm_normal_state_init(&normal, N, abc, (float[]){1, 0, 0, 0});
  imm_mute_state_init(&end, E, abc);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &normal.super);
  imm_hmm_add_state(hmm, &end.super);

  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_set_trans(hmm, &start.super, &normal.super, 2);
  imm_hmm_set_trans(hmm, &normal.super, &normal.super, 3);
  imm_hmm_set_trans(hmm, &normal.super, &end.super, 4);

  imm_hmm_dump(hmm, &state_name, stdout);

  static struct imm_dp dp = {0};
  imm_hmm_init_dp(hmm, &dp);

  struct imm_task *task = imm_task_new(&dp);

  struct imm_seq seq = imm_seq_unsafe(imm_str("A"), abc);
  imm_eseq_setup(&eseq, &seq);
  imm_task_setup(task, &eseq);
  imm_task_debug_setup(task, imm_seq_data(&seq));

  imm_dp_set_state_name(&dp, &state_name);
  imm_dp_viterbi(&dp, task, &prod);
  close(prod.loglik, 7);
  imm_path_dump(&prod.path, &state_name, &seq, stdout);
  close(imm_path_score(&prod.path), 7);
  imm_trellis_set_ids(&task->trellis, dp.state_table.ids);
  imm_trellis_set_state_name(&task->trellis, &state_name);
  imm_trellis_dump(&task->trellis, stdout);

  ASSERT_SEQ(prod.path, 0, S, 0, 0);
  ASSERT_SEQ(prod.path, 1, N, 1, 2);
  ASSERT_SEQ(prod.path, 2, E, 0, 7);

  imm_dp_cleanup(&dp);
  imm_task_del(task);
  cleanup();
}

static void states_sne_no_solution(void)
{
  setup();

  imm_mute_state_init(&start, S, abc);
  imm_normal_state_init(
      &normal, N, abc,
      (float[]){1, IMM_LPROB_ZERO, IMM_LPROB_ZERO, IMM_LPROB_ZERO});
  imm_mute_state_init(&end, E, abc);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &normal.super);
  imm_hmm_add_state(hmm, &end.super);

  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_set_trans(hmm, &start.super, &normal.super, 2);
  imm_hmm_set_trans(hmm, &normal.super, &normal.super, 3);
  imm_hmm_set_trans(hmm, &normal.super, &end.super, 4);

  imm_hmm_dump(hmm, &state_name, stdout);

  static struct imm_dp dp = {0};
  imm_hmm_init_dp(hmm, &dp);

  struct imm_task *task = imm_task_new(&dp);

  struct imm_seq seq = imm_seq_unsafe(imm_str("AC"), abc);
  imm_eseq_setup(&eseq, &seq);
  imm_task_setup(task, &eseq);
  imm_task_debug_setup(task, imm_seq_data(&seq));

  imm_dp_set_state_name(&dp, &state_name);
  imm_dp_viterbi(&dp, task, &prod);
  ok(imm_lprob_is_nan(prod.loglik));
  imm_path_dump(&prod.path, &state_name, &seq, stdout);
  ok(imm_lprob_is_nan(imm_path_score(&prod.path)));
  imm_trellis_set_ids(&task->trellis, dp.state_table.ids);
  imm_trellis_set_state_name(&task->trellis, &state_name);
  imm_trellis_dump(&task->trellis, stdout);

  eq(imm_path_nsteps(&prod.path), 0);

  imm_dp_cleanup(&dp);
  imm_task_del(task);
  cleanup();
}

static float table_lprob(int size, char const *seq)
{
  if (size == 1 && strncmp(seq, "A", (unsigned)size) == 0) return 10;
  else if (size == 2 && strncmp(seq, "GC", (unsigned)size) == 0) return 15;
  return IMM_LPROB_ZERO;
}

static void states_ste(void)
{
  setup();

  imm_mute_state_init(&start, S, abc);
  imm_table_state_init(&table, T, abc, &table_lprob, imm_span(1, 2));
  imm_mute_state_init(&end, E, abc);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &table.super);
  imm_hmm_add_state(hmm, &end.super);

  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_set_trans(hmm, &start.super, &table.super, 2);
  imm_hmm_set_trans(hmm, &table.super, &table.super, 3);
  imm_hmm_set_trans(hmm, &table.super, &end.super, 4);

  imm_hmm_dump(hmm, &state_name, stdout);

  static struct imm_dp dp = {0};
  imm_hmm_init_dp(hmm, &dp);

  struct imm_task *task = imm_task_new(&dp);

  struct imm_seq seq = imm_seq_unsafe(imm_str("AGC"), abc);
  imm_eseq_setup(&eseq, &seq);
  imm_task_setup(task, &eseq);
  imm_task_debug_setup(task, imm_seq_data(&seq));

  imm_dp_set_state_name(&dp, &state_name);
  imm_dp_viterbi(&dp, task, &prod);
  close(prod.loglik, 34);
  imm_path_dump(&prod.path, &state_name, &seq, stdout);
  close(imm_path_score(&prod.path), 34);
  imm_trellis_set_ids(&task->trellis, dp.state_table.ids);
  imm_trellis_set_state_name(&task->trellis, &state_name);
  imm_trellis_dump(&task->trellis, stdout);

  ASSERT_SEQ(prod.path, 0, S, 0, 0);
  ASSERT_SEQ(prod.path, 1, T, 1, 2);
  ASSERT_SEQ(prod.path, 2, T, 2, 15);
  ASSERT_SEQ(prod.path, 3, E, 0, 34);

  imm_dp_cleanup(&dp);
  imm_task_del(task);
  cleanup();
}

static void states_ste_2(void)
{
  setup();

  imm_mute_state_init(&start, S, abc);
  imm_table_state_init(&table, T, abc, &table_lprob, imm_span(1, 2));
  imm_mute_state_init(&end, E, abc);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &table.super);
  imm_hmm_add_state(hmm, &end.super);

  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_set_trans(hmm, &start.super, &table.super, 2);
  imm_hmm_set_trans(hmm, &table.super, &table.super, 3);
  imm_hmm_set_trans(hmm, &table.super, &end.super, 4);

  imm_hmm_dump(hmm, &state_name, stdout);

  static struct imm_dp dp = {0};
  imm_hmm_init_dp(hmm, &dp);

  struct imm_task *task = imm_task_new(&dp);

  struct imm_seq seq = imm_seq_unsafe(imm_str("AGCA"), abc);
  imm_eseq_setup(&eseq, &seq);
  imm_task_setup(task, &eseq);
  imm_task_debug_setup(task, imm_seq_data(&seq));

  imm_dp_set_state_name(&dp, &state_name);
  imm_dp_viterbi(&dp, task, &prod);
  close(prod.loglik, 47);
  imm_path_dump(&prod.path, &state_name, &seq, stdout);
  close(imm_path_score(&prod.path), 47);
  imm_trellis_set_ids(&task->trellis, dp.state_table.ids);
  imm_trellis_set_state_name(&task->trellis, &state_name);
  imm_trellis_dump(&task->trellis, stdout);

  ASSERT_SEQ(prod.path, 0, S, 0, 0);
  ASSERT_SEQ(prod.path, 1, T, 1, 2);
  ASSERT_SEQ(prod.path, 2, T, 2, 15);
  ASSERT_SEQ(prod.path, 3, T, 1, 33);
  ASSERT_SEQ(prod.path, 4, E, 0, 47);

  imm_dp_cleanup(&dp);
  imm_task_del(task);
  cleanup();
}

int main(void)
{
  lrun("states_sne", states_sne);
  lrun("states_sne_no_solution", states_sne_no_solution);
  lrun("states_ste", states_ste);
  lrun("states_ste_2", states_ste_2);
  return lfails != 0;
}
