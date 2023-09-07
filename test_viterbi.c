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
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code_ab);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code_ab);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc_ab);
  struct imm_mute_state D0;
  imm_mute_state_init(&D0, 1, &abc_ab);
  struct imm_mute_state end;
  imm_mute_state_init(&end, 2, &abc_ab);

  float M0_lprobs[] = {log(0.4), log(0.2)};
  struct imm_normal_state M0;
  imm_normal_state_init(&M0, 3, &abc_ab, M0_lprobs);

  float I0_lprobs[] = {log(0.5), log(0.5)};
  struct imm_normal_state I0;
  imm_normal_state_init(&I0, 4, &abc_ab, I0_lprobs);

  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_add_state(&hmm, &D0.super);
  imm_hmm_add_state(&hmm, &end.super);

  imm_hmm_set_start(&hmm, &start);
  imm_hmm_set_end(&hmm, &end);

  imm_hmm_add_state(&hmm, &M0.super);
  imm_hmm_add_state(&hmm, &I0.super);

  imm_hmm_set_trans(&hmm, &start.super, &D0.super, log(0.1));
  imm_hmm_set_trans(&hmm, &D0.super, &end.super, log(1.0));
  imm_hmm_set_trans(&hmm, &start.super, &M0.super, log(0.5));
  imm_hmm_set_trans(&hmm, &M0.super, &end.super, log(0.8));
  imm_hmm_set_trans(&hmm, &M0.super, &I0.super, log(0.1));
  imm_hmm_set_trans(&hmm, &I0.super, &I0.super, log(0.2));
  imm_hmm_set_trans(&hmm, &I0.super, &end.super, log(1.0));

  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &dp), 0);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.1) + log(1.0)));

  close(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path), log(0.1) + log(1.0));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.1) + log(1.0));

  imm_dp_cleanup(&dp);
  imm_hmm_set_end(&hmm, &end);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = log(0.5) + log(0.4) + log(0.8);
  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_set_end(&hmm, &end);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.5) + log(0.2) + log(0.8);
  eq(imm_eseq_setup(&eseq, &B_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &B_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), des);

  des = log(0.5) + log(0.4) + log(0.1) + log(0.5);
  eq(imm_eseq_setup(&eseq, &AA_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &AA_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), des);

  des = log(0.5) + log(0.4) + log(0.1) + log(0.2) + 2 * log(0.5);
  eq(imm_eseq_setup(&eseq, &AAB_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &AAB_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), des);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
}

static void profile2(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc);

  float ins_lprobs[] = {log(0.1), log(0.1), log(0.1), log(0.7)};

  float M0_lprobs[] = {log(0.4), zero(), log(0.6), zero()};
  float M1_lprobs[] = {log(0.6), zero(), log(0.4), zero()};
  float M2_lprobs[] = {log(0.05), log(0.05), log(0.05), log(0.05)};

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

  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_set_start(&hmm, &start);

  imm_hmm_add_state(&hmm, &M0.super);
  imm_hmm_add_state(&hmm, &I0.super);

  imm_hmm_add_state(&hmm, &D1.super);
  imm_hmm_add_state(&hmm, &M1.super);
  imm_hmm_add_state(&hmm, &I1.super);

  imm_hmm_add_state(&hmm, &D2.super);
  imm_hmm_add_state(&hmm, &M2.super);

  imm_hmm_add_state(&hmm, &end.super);
  imm_hmm_set_end(&hmm, &end);

  imm_hmm_set_trans(&hmm, &start.super, &M0.super, 0.0);
  imm_hmm_set_trans(&hmm, &start.super, &M1.super, 0.0);
  imm_hmm_set_trans(&hmm, &start.super, &M2.super, 0.0);
  imm_hmm_set_trans(&hmm, &M0.super, &M1.super, 0.0);
  imm_hmm_set_trans(&hmm, &M0.super, &M2.super, 0.0);
  imm_hmm_set_trans(&hmm, &M0.super, &end.super, 0.0);
  imm_hmm_set_trans(&hmm, &M1.super, &M2.super, 0.0);
  imm_hmm_set_trans(&hmm, &M1.super, &end.super, 0.0);
  imm_hmm_set_trans(&hmm, &M2.super, &end.super, 0.0);

  imm_hmm_set_trans(&hmm, &M0.super, &I0.super, log(0.2));
  imm_hmm_set_trans(&hmm, &M0.super, &D1.super, log(0.1));
  imm_hmm_set_trans(&hmm, &I0.super, &I0.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I0.super, &M1.super, log(0.5));

  imm_hmm_set_trans(&hmm, &M1.super, &D2.super, log(0.1));
  imm_hmm_set_trans(&hmm, &M1.super, &I1.super, log(0.2));
  imm_hmm_set_trans(&hmm, &I1.super, &I1.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I1.super, &M2.super, log(0.5));
  imm_hmm_set_trans(&hmm, &D1.super, &D2.super, log(0.3));
  imm_hmm_set_trans(&hmm, &D1.super, &M2.super, log(0.7));

  imm_hmm_set_trans(&hmm, &D2.super, &end.super, log(1.0));

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.05));

  close(imm_hmm_loglik(&hmm, &C, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.05));

  close(imm_hmm_loglik(&hmm, &T, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.6));

  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6));

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.05));

  close(imm_hmm_loglik(&hmm, &C, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  eq(imm_eseq_setup(&eseq, &G), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.6));

  close(imm_hmm_loglik(&hmm, &G, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.05));

  close(imm_hmm_loglik(&hmm, &T, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, log(0.6));

  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &GA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, 2 * log(0.6));

  close(imm_hmm_loglik(&hmm, &GA, &prod.path), 2 * log(0.6));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), 2 * log(0.6));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6);
  eq(imm_eseq_setup(&eseq, &GTTTA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 7U);
  close(imm_path_score(&prod.path), des);

  des =
      log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) + log(0.05);
  eq(imm_eseq_setup(&eseq, &GTTTAC), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTAC, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 8U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) +
        log(0.6);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 9U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) +
        log(0.6);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 9U);
  close(imm_path_score(&prod.path), des);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

int main(void)
{
  imm_abc_init(&abc, IMM_STR("ACGT"), '*');
  imm_code_init(&code, &abc);
  EMPTY = imm_seq(IMM_STR(""), &abc);
  A = imm_seq(IMM_STR("A"), &abc);
  C = imm_seq(IMM_STR("C"), &abc);
  G = imm_seq(IMM_STR("G"), &abc);
  T = imm_seq(IMM_STR("T"), &abc);
  AA = imm_seq(IMM_STR("AA"), &abc);
  AC = imm_seq(IMM_STR("AC"), &abc);
  AG = imm_seq(IMM_STR("AG"), &abc);
  AT = imm_seq(IMM_STR("AT"), &abc);
  ACT = imm_seq(IMM_STR("ACT"), &abc);
  AGT = imm_seq(IMM_STR("AGT"), &abc);
  ATT = imm_seq(IMM_STR("ATT"), &abc);
  AGTC = imm_seq(IMM_STR("AGTC"), &abc);
  GA = imm_seq(IMM_STR("GA"), &abc);
  GT = imm_seq(IMM_STR("GT"), &abc);
  GTTTA = imm_seq(IMM_STR("GTTTA"), &abc);
  GTTTAC = imm_seq(IMM_STR("GTTTAC"), &abc);
  GTTTACA = imm_seq(IMM_STR("GTTTACA"), &abc);

  imm_abc_init(&abc_ab, IMM_STR("AB"), '*');
  imm_code_init(&code_ab, &abc_ab);
  EMPTY_ab = imm_seq(IMM_STR(""), &abc_ab);
  A_ab = imm_seq(IMM_STR("A"), &abc_ab);
  B_ab = imm_seq(IMM_STR("B"), &abc_ab);
  AA_ab = imm_seq(IMM_STR("AA"), &abc_ab);
  AB_ab = imm_seq(IMM_STR("AB"), &abc_ab);
  AAB_ab = imm_seq(IMM_STR("AAB"), &abc_ab);

  lrun("profile1", profile1);
  lrun("profile2", profile2);

  return lfails != 0;
}
