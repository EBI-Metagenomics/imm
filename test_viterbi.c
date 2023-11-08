#include "abc.h"
#include "code.h"
#include "dp.h"
#include "hmm.h"
#include "lprob.h"
#include "mute_state.h"
#include "normal_state.h"
#include "prod.h"
#include "seq.h"
#include "task.h"
#include "vendor/minctest.h"
#include <stdlib.h>

static struct imm_abc abc;
static struct imm_code code;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq C;
static struct imm_seq G;
static struct imm_seq T;
static struct imm_seq AA;
static struct imm_seq AC;
static struct imm_seq AG;
static struct imm_seq AT;
static struct imm_seq ACT;
static struct imm_seq AGT;
static struct imm_seq ATT;
static struct imm_seq AGTC;
static struct imm_seq GA;
static struct imm_seq GT;
static struct imm_seq GTTTA;
static struct imm_seq GTTTAC;
static struct imm_seq GTTTACA;

static struct imm_abc abc_ab;
static struct imm_code code_ab;
static struct imm_seq EMPTY_ab;
static struct imm_seq A_ab;
static struct imm_seq B_ab;
static struct imm_seq AA_ab;
static struct imm_seq AB_ab;
static struct imm_seq AAB_ab;

static inline float zero(void) { return imm_lprob_zero(); }

static void profile1(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code_ab);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code_ab);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc_ab);
  struct imm_mute_state D0;
  imm_mute_state_init(&D0, 1, &abc_ab);
  struct imm_mute_state end;
  imm_mute_state_init(&end, 2, &abc_ab);

  float M0_lprobs[] = {logf(0.4f), logf(0.2f)};
  struct imm_normal_state M0;
  imm_normal_state_init(&M0, 3, &abc_ab, M0_lprobs);

  float I0_lprobs[] = {logf(0.5f), logf(0.5f)};
  struct imm_normal_state I0;
  imm_normal_state_init(&I0, 4, &abc_ab, I0_lprobs);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &D0.super);
  imm_hmm_add_state(hmm, &end.super);

  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_add_state(hmm, &M0.super);
  imm_hmm_add_state(hmm, &I0.super);

  imm_hmm_set_trans(hmm, &start.super, &D0.super, logf(0.1f));
  imm_hmm_set_trans(hmm, &D0.super, &end.super, logf(1.0f));
  imm_hmm_set_trans(hmm, &start.super, &M0.super, logf(0.5f));
  imm_hmm_set_trans(hmm, &M0.super, &end.super, logf(0.8f));
  imm_hmm_set_trans(hmm, &M0.super, &I0.super, logf(0.1f));
  imm_hmm_set_trans(hmm, &I0.super, &I0.super, logf(0.2f));
  imm_hmm_set_trans(hmm, &I0.super, &end.super, logf(1.0f));

  struct imm_dp dp;
  eq(imm_hmm_init_dp(hmm, &dp), 0);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (logf(0.1f) + logf(1.0f)));

  close(imm_hmm_loglik(hmm, &EMPTY_ab, &prod.path), logf(0.1f) + logf(1.0f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.1f) + logf(1.0f));

  imm_dp_cleanup(&dp);
  imm_hmm_set_end(hmm, &end);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = logf(0.5f) + logf(0.4f) + logf(0.8f);
  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &A_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_set_end(hmm, &end);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = logf(0.5f) + logf(0.2f) + logf(0.8f);
  eq(imm_eseq_setup(&eseq, &B_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &B_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), des);

  des = logf(0.5f) + logf(0.4f) + logf(0.1f) + logf(0.5f);
  eq(imm_eseq_setup(&eseq, &AA_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &AA_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 4);
  close(imm_path_score(&prod.path), des);

  des = logf(0.5f) + logf(0.4f) + logf(0.1f) + logf(0.2f) + 2 * logf(0.5f);
  eq(imm_eseq_setup(&eseq, &AAB_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &AAB_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 5);
  close(imm_path_score(&prod.path), des);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
  imm_hmm_del(hmm);
}

static void profile2(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc);

  float ins_lprobs[] = {logf(0.1f), logf(0.1f), logf(0.1f), logf(0.7f)};

  float M0_lprobs[] = {logf(0.4f), zero(), logf(0.6f), zero()};
  float M1_lprobs[] = {logf(0.6f), zero(), logf(0.4f), zero()};
  float M2_lprobs[] = {logf(0.05f), logf(0.05f), logf(0.05f), logf(0.05f)};

  struct imm_normal_state M0;
  imm_normal_state_init(&M0, 1, &abc, M0_lprobs);
  struct imm_normal_state I0;
  imm_normal_state_init(&I0, 2, &abc, ins_lprobs);

  struct imm_mute_state D1;
  imm_mute_state_init(&D1, 3, &abc);
  struct imm_normal_state M1;
  imm_normal_state_init(&M1, 4, &abc, M1_lprobs);
  struct imm_normal_state I1;
  imm_normal_state_init(&I1, 5, &abc, ins_lprobs);

  struct imm_mute_state D2;
  imm_mute_state_init(&D2, 6, &abc);
  struct imm_normal_state M2;
  imm_normal_state_init(&M2, 7, &abc, M2_lprobs);

  struct imm_mute_state end;
  imm_mute_state_init(&end, 8, &abc);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_set_start(hmm, &start);

  imm_hmm_add_state(hmm, &M0.super);
  imm_hmm_add_state(hmm, &I0.super);

  imm_hmm_add_state(hmm, &D1.super);
  imm_hmm_add_state(hmm, &M1.super);
  imm_hmm_add_state(hmm, &I1.super);

  imm_hmm_add_state(hmm, &D2.super);
  imm_hmm_add_state(hmm, &M2.super);

  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_end(hmm, &end);

  imm_hmm_set_trans(hmm, &start.super, &M0.super, 0.0);
  imm_hmm_set_trans(hmm, &start.super, &M1.super, 0.0);
  imm_hmm_set_trans(hmm, &start.super, &M2.super, 0.0);
  imm_hmm_set_trans(hmm, &M0.super, &M1.super, 0.0);
  imm_hmm_set_trans(hmm, &M0.super, &M2.super, 0.0);
  imm_hmm_set_trans(hmm, &M0.super, &end.super, 0.0);
  imm_hmm_set_trans(hmm, &M1.super, &M2.super, 0.0);
  imm_hmm_set_trans(hmm, &M1.super, &end.super, 0.0);
  imm_hmm_set_trans(hmm, &M2.super, &end.super, 0.0);

  imm_hmm_set_trans(hmm, &M0.super, &I0.super, logf(0.2f));
  imm_hmm_set_trans(hmm, &M0.super, &D1.super, logf(0.1f));
  imm_hmm_set_trans(hmm, &I0.super, &I0.super, logf(0.5f));
  imm_hmm_set_trans(hmm, &I0.super, &M1.super, logf(0.5f));

  imm_hmm_set_trans(hmm, &M1.super, &D2.super, logf(0.1f));
  imm_hmm_set_trans(hmm, &M1.super, &I1.super, logf(0.2f));
  imm_hmm_set_trans(hmm, &I1.super, &I1.super, logf(0.5f));
  imm_hmm_set_trans(hmm, &I1.super, &M2.super, logf(0.5f));
  imm_hmm_set_trans(hmm, &D1.super, &D2.super, logf(0.3f));
  imm_hmm_set_trans(hmm, &D1.super, &M2.super, logf(0.7f));

  imm_hmm_set_trans(hmm, &D2.super, &end.super, logf(1.0f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.05f));

  close(imm_hmm_loglik(hmm, &C, &prod.path), logf(0.05f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.05f));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.05f));

  close(imm_hmm_loglik(hmm, &T, &prod.path), logf(0.05f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.05f));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.6f));

  close(imm_hmm_loglik(hmm, &A, &prod.path), logf(0.6f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.6f));

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.05f));

  close(imm_hmm_loglik(hmm, &C, &prod.path), logf(0.05f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.05f));

  eq(imm_eseq_setup(&eseq, &G), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.6f));

  close(imm_hmm_loglik(hmm, &G, &prod.path), logf(0.6f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.6f));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.05f));

  close(imm_hmm_loglik(hmm, &T, &prod.path), logf(0.05f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.05f));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, logf(0.6f));

  close(imm_hmm_loglik(hmm, &A, &prod.path), logf(0.6f));
  eq(imm_path_nsteps(&prod.path), 3);
  close(imm_path_score(&prod.path), logf(0.6f));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &GA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, 2 * logf(0.6f));

  close(imm_hmm_loglik(hmm, &GA, &prod.path), 2 * logf(0.6f));
  eq(imm_path_nsteps(&prod.path), 4);
  close(imm_path_score(&prod.path), 2 * logf(0.6f));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des =
      logf(0.6f) + logf(0.2f) + 3 * logf(0.7f) + 3 * logf(0.5f) + logf(0.6f);
  eq(imm_eseq_setup(&eseq, &GTTTA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &GTTTA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 7);
  close(imm_path_score(&prod.path), des);

  des = logf(0.6f) + logf(0.2f) + 3 * logf(0.7f) + 3 * logf(0.5f) + logf(0.6f) +
        logf(0.05f);
  eq(imm_eseq_setup(&eseq, &GTTTAC), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &GTTTAC, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 8);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = logf(0.6f) + logf(0.2f) + 5 * logf(0.5f) + 3 * logf(0.7f) +
        2 * logf(0.1f) + logf(0.6f);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 9);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = logf(0.6f) + logf(0.2f) + 5 * logf(0.5f) + 3 * logf(0.7f) +
        2 * logf(0.1f) + logf(0.6f);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 9);
  close(imm_path_score(&prod.path), des);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_hmm_del(hmm);
}

int main(void)
{
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  imm_code_init(&code, &abc);
  EMPTY = imm_seq_unsafe(imm_str(""), &abc);
  A = imm_seq_unsafe(imm_str("A"), &abc);
  C = imm_seq_unsafe(imm_str("C"), &abc);
  G = imm_seq_unsafe(imm_str("G"), &abc);
  T = imm_seq_unsafe(imm_str("T"), &abc);
  AA = imm_seq_unsafe(imm_str("AA"), &abc);
  AC = imm_seq_unsafe(imm_str("AC"), &abc);
  AG = imm_seq_unsafe(imm_str("AG"), &abc);
  AT = imm_seq_unsafe(imm_str("AT"), &abc);
  ACT = imm_seq_unsafe(imm_str("ACT"), &abc);
  AGT = imm_seq_unsafe(imm_str("AGT"), &abc);
  ATT = imm_seq_unsafe(imm_str("ATT"), &abc);
  AGTC = imm_seq_unsafe(imm_str("AGTC"), &abc);
  GA = imm_seq_unsafe(imm_str("GA"), &abc);
  GT = imm_seq_unsafe(imm_str("GT"), &abc);
  GTTTA = imm_seq_unsafe(imm_str("GTTTA"), &abc);
  GTTTAC = imm_seq_unsafe(imm_str("GTTTAC"), &abc);
  GTTTACA = imm_seq_unsafe(imm_str("GTTTACA"), &abc);

  imm_abc_init(&abc_ab, imm_str("AB"), '*');
  imm_code_init(&code_ab, &abc_ab);
  EMPTY_ab = imm_seq_unsafe(imm_str(""), &abc_ab);
  A_ab = imm_seq_unsafe(imm_str("A"), &abc_ab);
  B_ab = imm_seq_unsafe(imm_str("B"), &abc_ab);
  AA_ab = imm_seq_unsafe(imm_str("AA"), &abc_ab);
  AB_ab = imm_seq_unsafe(imm_str("AB"), &abc_ab);
  AAB_ab = imm_seq_unsafe(imm_str("AAB"), &abc_ab);

  lrun("profile1", profile1);
  lrun("profile2", profile2);

  return lfails != 0;
}
