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

static void one_mute_state(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state state;
  imm_mute_state_init(&state, 0, &abc);

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  {
    eq(imm_eseq_setup(&eseq, &EMPTY), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, log(1.0));
    close(imm_hmm_loglik(&hmm, &EMPTY, &prod.path), log(1.0));
    eq(imm_path_nsteps(&prod.path), 1U);
    close(imm_path_score(&prod.path), log(1.0));

    eq(imm_eseq_setup(&eseq, &C), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &C, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
    ok(imm_lprob_is_nan(imm_path_score(&prod.path)));
  }

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
}

static void two_mute_states(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);

  imm_hmm_add_state(&hmm, &state0.super);
  imm_hmm_set_start(&hmm, &state0);
  imm_hmm_add_state(&hmm, &state1.super);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  {
    eq(imm_eseq_setup(&eseq, &EMPTY), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, log(1.0));
    close(imm_hmm_loglik(&hmm, &EMPTY, &prod.path), log(1.0));
    eq(imm_path_nsteps(&prod.path), 1U);
    close(imm_path_score(&prod.path), log(1.0));
  }

  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.1));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(1.0) + log(0.1));
  eq(imm_path_step(&prod.path, 0)->state_id, 0);
  eq(imm_path_step(&prod.path, 0)->seqlen, 0);
  eq(imm_path_step(&prod.path, 1)->state_id, 1);
  eq(imm_path_step(&prod.path, 1)->seqlen, 0);
  close(imm_hmm_loglik(&hmm, &EMPTY, &prod.path), log(1.0) + log(0.1));
  close(prod.loglik, log(1.0) + log(0.1));

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

static void mute_cycle(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);

  imm_hmm_add_state(&hmm, &state0.super);
  imm_hmm_set_start(&hmm, &state0);

  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 1, &abc);
  imm_hmm_add_state(&hmm, &state1.super);

  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.2));
  imm_hmm_set_trans(&hmm, &state1.super, &state0.super, log(0.2));

  struct imm_dp dp;
  eq(imm_hmm_init_dp(&hmm, &state0.super, &dp), IMM_EMUTECYLES);
}

static void one_normal_state(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  float lprobs0[] = {log(0.25), log(0.25), log(1.0), imm_lprob_zero()};
  struct imm_normal_state state;
  imm_normal_state_init(&state, 0, &abc, lprobs0);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &abc);

  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_trans(&hmm, &start.super, &state.super, 0);
  imm_hmm_set_start(&hmm, &start);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &EMPTY), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ESHORTSEQ);

  {
    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (log(1.0) + log(0.25)));
    close(imm_hmm_loglik(&hmm, &A, &prod.path), (log(1.0) + log(0.25)));
    eq(imm_path_nsteps(&prod.path), 2U);
    close(imm_path_score(&prod.path), log(1.0) + log(0.25));
  }

  {
    eq(imm_eseq_setup(&eseq, &T), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &T, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  {
    eq(imm_eseq_setup(&eseq, &AC), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &AC, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  imm_hmm_set_trans(&hmm, &state.super, &state.super, log(0.1));
  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  {
    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (log(1.0) + log(0.25)));
    close(imm_hmm_loglik(&hmm, &A, &prod.path), log(1.0) + log(0.25));
    close(imm_path_score(&prod.path), log(1.0) + log(0.25));
    eq(imm_path_nsteps(&prod.path), 2U);
  }

  {
    eq(imm_eseq_setup(&eseq, &AA), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (log(0.1) + 2 * log(0.25)));
    close(imm_hmm_loglik(&hmm, &AA, &prod.path), log(0.1) + 2 * log(0.25));
    close(imm_path_score(&prod.path), log(0.1) + 2 * log(0.25));
    eq(imm_path_nsteps(&prod.path), 3U);
  }

  {
    eq(imm_eseq_setup(&eseq, &ACT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &ACT, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  imm_hmm_normalize_trans(&hmm);
  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  {

    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, log(0.25));
    close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.25));
    close(imm_path_score(&prod.path), log(0.25));
    eq(imm_path_nsteps(&prod.path), 2U);
  }

  {
    eq(imm_eseq_setup(&eseq, &AA), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (2 * log(0.25)));
    close(imm_hmm_loglik(&hmm, &AA, &prod.path), 2 * log(0.25));
    close(imm_path_score(&prod.path), 2 * log(0.25));
    eq(imm_path_nsteps(&prod.path), 3U);
  }

  {
    eq(imm_eseq_setup(&eseq, &ACT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &ACT, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

static void two_normal_states(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  float lprobs0[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
  struct imm_normal_state state0;
  imm_normal_state_init(&state0, 0, &abc, lprobs0);

  float lprobs1[] = {log(0.25), log(0.25), log(0.5), log(0.5)};
  struct imm_normal_state state1;
  imm_normal_state_init(&state1, 1, &abc, lprobs1);

  struct imm_mute_state start0 = {0};
  imm_mute_state_init(&start0, 2, &abc);

  struct imm_mute_state start1 = {0};
  imm_mute_state_init(&start1, 3, &abc);

  imm_hmm_add_state(&hmm, &start0.super);
  imm_hmm_add_state(&hmm, &state0.super);
  imm_hmm_set_trans(&hmm, &start0.super, &state0.super, 0);
  imm_hmm_set_start(&hmm, &start0);
  imm_hmm_add_state(&hmm, &state1.super);
  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.3));

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  {
    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (log(1.0) + log(0.25)));
    close(imm_hmm_loglik(&hmm, &A, &prod.path), log(1.0) + log(0.25));
    eq(imm_path_nsteps(&prod.path), 2U);
    close(imm_path_score(&prod.path), log(1.0) + log(0.25));
  }

  {
    eq(imm_eseq_setup(&eseq, &T), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &T, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  {
    eq(imm_eseq_setup(&eseq, &AC), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &AC, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = log(1.0) + log(0.25) + log(0.3) + log(0.5);
  {
    eq(imm_eseq_setup(&eseq, &AT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, des);
    close(imm_hmm_loglik(&hmm, &AT, &prod.path), des);
    eq(imm_path_nsteps(&prod.path), 3U);
    close(imm_path_score(&prod.path), des);
  }

  {
    eq(imm_eseq_setup(&eseq, &ATT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &ATT, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  imm_hmm_add_state(&hmm, &start1.super);
  imm_hmm_set_trans(&hmm, &state1.super, &state1.super, log(0.5));
  imm_hmm_set_trans(&hmm, &start1.super, &state1.super, 0);
  imm_hmm_set_start(&hmm, &start1);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(1.0) + log(0.25) + 4 * log(0.5);
  {
    eq(imm_eseq_setup(&eseq, &ATT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, des);
    close(imm_hmm_loglik(&hmm, &ATT, &prod.path), des);
    eq(imm_path_nsteps(&prod.path), 4U);
    close(imm_path_score(&prod.path), des);
  }

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

static void normal_states(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  float const lprobs0[] = {log(0.25), log(0.25), log(0.5), imm_lprob_zero()};
  struct imm_normal_state state0;
  imm_normal_state_init(&state0, 0, &abc, lprobs0);

  float const lprobs1[] = {log(0.5) - log(2.25), log(0.25) - log(2.25),
                           log(0.5) - log(2.25), log(1.0) - log(2.25)};
  struct imm_normal_state state1;
  imm_normal_state_init(&state1, 1, &abc, lprobs1);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 2, &abc);

  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_add_state(&hmm, &state0.super);
  imm_hmm_set_trans(&hmm, &start.super, &state0.super, 0);
  imm_hmm_set_start(&hmm, &start);
  imm_hmm_add_state(&hmm, &state1.super);

  imm_hmm_set_trans(&hmm, &state0.super, &state0.super, log(0.1));
  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.2));
  imm_hmm_set_trans(&hmm, &state1.super, &state1.super, log(1.0));

  imm_hmm_normalize_trans(&hmm);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  {
    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, -1.386294361120);
    close(imm_hmm_loglik(&hmm, &A, &prod.path), -1.386294361120);
    eq(imm_path_nsteps(&prod.path), 2U);
  }

  {
    eq(imm_eseq_setup(&eseq, &AG), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, -3.178053830348);
    close(imm_hmm_loglik(&hmm, &AG, &prod.path), -3.178053830348);
    eq(imm_path_nsteps(&prod.path), 3U);
  }

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  {
    eq(imm_eseq_setup(&eseq, &AG), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, -3.295836866004);
    close(imm_hmm_loglik(&hmm, &AG, &prod.path), -3.295836866004);
    eq(imm_path_nsteps(&prod.path), 3U);
  }

  {
    imm_dp_cleanup(&dp);
    imm_hmm_init_dp(&hmm, &state0.super, &dp);
    eq(imm_task_reset(task, &dp), 0);
  }

  {
    eq(imm_eseq_setup(&eseq, &AGT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &AGT, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  {
    imm_dp_cleanup(&dp);
    imm_hmm_init_dp(&hmm, &state1.super, &dp);
    eq(imm_task_reset(task, &dp), 0);
  }

  {
    eq(imm_eseq_setup(&eseq, &AGT), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, -4.106767082221);
    close(imm_hmm_loglik(&hmm, &AGT, &prod.path), -4.106767082221);
    eq(imm_path_nsteps(&prod.path), 4U);
    close(imm_path_score(&prod.path), -4.106767082221);
  }

  {
    imm_dp_cleanup(&dp);
    imm_hmm_init_dp(&hmm, &state0.super, &dp);
    eq(imm_task_reset(task, &dp), 0);
  }

  {
    eq(imm_eseq_setup(&eseq, &AGTC), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    ok(imm_lprob_is_nan(prod.loglik));
    ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &AGTC, &prod.path)));
    eq(imm_path_nsteps(&prod.path), 0U);
  }

  {
    imm_dp_cleanup(&dp);
    imm_hmm_init_dp(&hmm, &state1.super, &dp);
    eq(imm_task_reset(task, &dp), 0);
  }

  {
    eq(imm_eseq_setup(&eseq, &AGTC), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, -6.303991659557);
    close(imm_hmm_loglik(&hmm, &AGTC, &prod.path), -6.303991659557);
    eq(imm_path_nsteps(&prod.path), 5U);
    close(imm_path_score(&prod.path), -6.303991659557);
  }

  eq(imm_hmm_set_trans(&hmm, &state0.super, &state0.super, zero()), IMM_EINVAL);
  eq(imm_hmm_set_trans(&hmm, &state0.super, &state1.super, zero()), IMM_EINVAL);
  eq(imm_hmm_set_trans(&hmm, &state1.super, &state0.super, zero()), IMM_EINVAL);
  eq(imm_hmm_set_trans(&hmm, &state1.super, &state1.super, zero()), IMM_EINVAL);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ESHORTSEQ);
  eq(imm_path_nsteps(&prod.path), 0U);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ESHORTSEQ);
  eq(imm_path_nsteps(&prod.path), 0U);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  {
    eq(imm_eseq_setup(&eseq, &A), 0);
    eq(imm_task_setup(task, &eseq), 0);
    eq(imm_dp_viterbi(&dp, task, &prod), 0);
    close(prod.loglik, (log(0.25)));
    close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.25));
    eq(imm_path_nsteps(&prod.path), 2U);
    close(imm_path_score(&prod.path), log(0.25));
  }

  imm_hmm_set_trans(&hmm, &state0.super, &state0.super, log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.25)));
  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.25));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.25));

  eq(imm_eseq_setup(&eseq, &AA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.25) + log(0.9)));
  close(imm_hmm_loglik(&hmm, &AA, &prod.path), 2 * log(0.25) + log(0.9));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), 2 * log(0.25) + log(0.9));

  imm_hmm_set_trans(&hmm, &state0.super, &state1.super, log(0.2));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &state0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.25)));
  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.25));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.25));

  eq(imm_eseq_setup(&eseq, &AA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.25) + log(0.9)));
  close(imm_hmm_loglik(&hmm, &AA, &prod.path), 2 * log(0.25) + log(0.9));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), 2 * log(0.25) + log(0.9));

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

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
  imm_hmm_set_start(&hmm, &start);
  imm_hmm_add_state(&hmm, &D0.super);
  imm_hmm_add_state(&hmm, &end.super);

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
  imm_hmm_init_dp(&hmm, &end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.1) + log(1.0)));

  close(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path), log(0.1) + log(1.0));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.1) + log(1.0));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &D0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.1)));

  close(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path), log(0.1));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.1));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &start.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(1.0)));

  close(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path), log(1.0));
  eq(imm_path_nsteps(&prod.path), 1U);
  close(imm_path_score(&prod.path), log(1.0));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &EMPTY_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), IMM_ESHORTSEQ);
  eq(imm_path_nsteps(&prod.path), 0U);
  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &EMPTY_ab, &prod.path)));
  ok(imm_lprob_is_nan(prod.loglik));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = log(0.5) + log(0.4);
  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.5) + log(0.4) + log(0.8);
  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &B_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.5) + log(0.2)));

  close(imm_hmm_loglik(&hmm, &B_ab, &prod.path), log(0.5) + log(0.2));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.5) + log(0.2));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
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
  imm_hmm_init_dp(&hmm, &M2.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.05));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M2.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &C, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.05));

  eq(imm_eseq_setup(&eseq, &G), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &G, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.05));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &T, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.05));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.6)));

  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6));

  eq(imm_eseq_setup(&eseq, &C), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &C, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  eq(imm_eseq_setup(&eseq, &G), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.6)));

  close(imm_hmm_loglik(&hmm, &G, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6));

  eq(imm_eseq_setup(&eseq, &T), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.05)));

  close(imm_hmm_loglik(&hmm, &T, &prod.path), log(0.05));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.05));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.6)));

  close(imm_hmm_loglik(&hmm, &A, &prod.path), log(0.6));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.6));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &GA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.6)));

  close(imm_hmm_loglik(&hmm, &GA, &prod.path), 2 * log(0.6));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), 2 * log(0.6));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &I0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &GT), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.6) + log(0.2) + log(0.7)));

  close(imm_hmm_loglik(&hmm, &GT, &prod.path), log(0.6) + log(0.2) + log(0.7));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(0.6) + log(0.2) + log(0.7));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
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
  imm_hmm_init_dp(&hmm, &M2.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.6) + log(0.2) + 3 * log(0.7) + 3 * log(0.5) + log(0.6) +
        log(0.2) + log(0.1) + log(0.5) + log(0.05);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 8U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.6) + log(0.2) + 5 * log(0.5) + 3 * log(0.7) + 2 * log(0.1) +
        log(0.6);
  eq(imm_eseq_setup(&eseq, &GTTTACA), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &GTTTACA, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 8U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &end.super, &dp);
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

static void profile_delete(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code_ab);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code_ab);

  float N0_lprobs[] = {log(0.5), zero()};
  struct imm_normal_state N0;
  imm_normal_state_init(&N0, 0, &abc_ab, N0_lprobs);

  struct imm_mute_state M;
  imm_mute_state_init(&M, 1, &abc_ab);

  float N1_lprobs[] = {log(0.5), zero()};
  struct imm_normal_state N1;
  imm_normal_state_init(&N1, 2, &abc_ab, N1_lprobs);

  float N2_lprobs[] = {zero(), log(0.5)};
  struct imm_normal_state N2;
  imm_normal_state_init(&N2, 3, &abc_ab, N2_lprobs);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 4, &abc_ab);

  imm_hmm_add_state(&hmm, &N2.super);
  imm_hmm_add_state(&hmm, &N1.super);
  imm_hmm_add_state(&hmm, &M.super);
  imm_hmm_add_state(&hmm, &N0.super);
  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_set_trans(&hmm, &start.super, &N0.super, 0);
  imm_hmm_set_start(&hmm, &start);

  imm_hmm_set_trans(&hmm, &N0.super, &N1.super, log(0.5));
  imm_hmm_set_trans(&hmm, &N0.super, &M.super, log(0.5));
  imm_hmm_set_trans(&hmm, &N1.super, &N2.super, log(0.5));
  imm_hmm_set_trans(&hmm, &M.super, &N2.super, log(0.5));

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &N0.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.5)));

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), log(0.5));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(0.5));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.5)));

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), 2 * log(0.5));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), 2 * log(0.5));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &N2.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &AB_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (4 * log(0.5)));

  close(imm_hmm_loglik(&hmm, &AB_ab, &prod.path), 4 * log(0.5));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), 4 * log(0.5));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.5)));

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), 2 * log(0.5));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), 2 * log(0.5));

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

static void global_profile(void)
{
  struct imm_abc abc_z;
  struct imm_code code_z;
  struct imm_seq AA_z;
  struct imm_seq AAB_z;
  struct imm_seq C_z;
  struct imm_seq CC_z;
  struct imm_seq CCC_z;
  struct imm_seq CCA_z;
  struct imm_seq CCAB_z;
  struct imm_seq CCABB_z;
  struct imm_seq CCABA_z;

  imm_abc_init(&abc_z, IMM_STR("ABCZ"), '*');
  imm_code_init(&code_z, &abc_z);
  AA_z = imm_seq(IMM_STR("AA"), &abc_z);
  AAB_z = imm_seq(IMM_STR("AAB"), &abc_z);
  C_z = imm_seq(IMM_STR("C"), &abc_z);
  CC_z = imm_seq(IMM_STR("CC"), &abc_z);
  CCC_z = imm_seq(IMM_STR("CCC"), &abc_z);
  CCA_z = imm_seq(IMM_STR("CCA"), &abc_z);
  CCAB_z = imm_seq(IMM_STR("CCAB"), &abc_z);
  CCABB_z = imm_seq(IMM_STR("CCABB"), &abc_z);
  CCABA_z = imm_seq(IMM_STR("CCABA"), &abc_z);

  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code_z);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code_z);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc_z);

  float B_lprobs[] = {log(0.01), log(0.01), log(1.0), zero()};
  struct imm_normal_state B;
  imm_normal_state_init(&B, 1, &abc_z, B_lprobs);

  float M0_lprobs[] = {log(0.9), log(0.01), log(0.01), zero()};

  struct imm_normal_state M0;
  imm_normal_state_init(&M0, 2, &abc_z, M0_lprobs);

  float M1_lprobs[] = {log(0.01), log(0.9), zero(), zero()};
  struct imm_normal_state M1;
  imm_normal_state_init(&M1, 3, &abc_z, M1_lprobs);

  float M2_lprobs[] = {log(0.5), log(0.5), zero(), zero()};
  struct imm_normal_state M2;
  imm_normal_state_init(&M2, 4, &abc_z, M2_lprobs);

  struct imm_mute_state E;
  imm_mute_state_init(&E, 5, &abc_z);
  struct imm_mute_state end;
  imm_mute_state_init(&end, 6, &abc_z);

  float Z_lprobs[] = {zero(), zero(), zero(), log(1.0)};
  struct imm_normal_state Z;
  imm_normal_state_init(&Z, 7, &abc_z, Z_lprobs);

  float ins_lprobs[] = {log(0.1), log(0.1), log(0.1), zero()};
  struct imm_normal_state I0;
  imm_normal_state_init(&I0, 8, &abc_z, ins_lprobs);
  struct imm_normal_state I1;
  imm_normal_state_init(&I1, 9, &abc_z, ins_lprobs);

  struct imm_mute_state D1;
  imm_mute_state_init(&D1, 10, &abc_z);
  struct imm_mute_state D2;
  imm_mute_state_init(&D2, 11, &abc_z);

  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_set_start(&hmm, &start);
  imm_hmm_add_state(&hmm, &B.super);
  imm_hmm_add_state(&hmm, &M0.super);
  imm_hmm_add_state(&hmm, &M1.super);
  imm_hmm_add_state(&hmm, &M2.super);
  imm_hmm_add_state(&hmm, &D1.super);
  imm_hmm_add_state(&hmm, &D2.super);
  imm_hmm_add_state(&hmm, &I0.super);
  imm_hmm_add_state(&hmm, &I1.super);
  imm_hmm_add_state(&hmm, &E.super);
  imm_hmm_add_state(&hmm, &Z.super);
  imm_hmm_add_state(&hmm, &end.super);

  imm_hmm_set_trans(&hmm, &start.super, &B.super, 0);
  imm_hmm_set_trans(&hmm, &B.super, &B.super, 0);
  imm_hmm_set_trans(&hmm, &B.super, &M0.super, 0);
  imm_hmm_set_trans(&hmm, &B.super, &M1.super, 0);
  imm_hmm_set_trans(&hmm, &B.super, &M2.super, 0);
  imm_hmm_set_trans(&hmm, &M0.super, &M1.super, 0);
  imm_hmm_set_trans(&hmm, &M1.super, &M2.super, 0);
  imm_hmm_set_trans(&hmm, &M2.super, &E.super, 0);
  imm_hmm_set_trans(&hmm, &M0.super, &E.super, 0);
  imm_hmm_set_trans(&hmm, &M1.super, &E.super, 0);

  imm_hmm_set_trans(&hmm, &E.super, &end.super, 0);

  imm_hmm_set_trans(&hmm, &E.super, &Z.super, 0);
  imm_hmm_set_trans(&hmm, &Z.super, &Z.super, 0);
  imm_hmm_set_trans(&hmm, &Z.super, &B.super, 0);

  imm_hmm_set_trans(&hmm, &M0.super, &D1.super, 0);
  imm_hmm_set_trans(&hmm, &D1.super, &D2.super, 0);
  imm_hmm_set_trans(&hmm, &D1.super, &M2.super, 0);
  imm_hmm_set_trans(&hmm, &D2.super, &E.super, 0);

  imm_hmm_set_trans(&hmm, &M0.super, &I0.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I0.super, &I0.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I0.super, &M1.super, log(0.5));

  imm_hmm_set_trans(&hmm, &M1.super, &I1.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I1.super, &I1.super, log(0.5));
  imm_hmm_set_trans(&hmm, &I1.super, &M2.super, log(0.5));

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &start.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &C_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  ok(imm_lprob_is_nan(prod.loglik));

  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &C_z, &prod.path)));
  eq(imm_path_nsteps(&prod.path), 0U);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &B.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &C_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(1.0)));

  close(imm_hmm_loglik(&hmm, &C_z, &prod.path), log(1.0));
  eq(imm_path_nsteps(&prod.path), 2U);
  close(imm_path_score(&prod.path), log(1.0));

  eq(imm_eseq_setup(&eseq, &CC_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(1.0)));

  close(imm_hmm_loglik(&hmm, &CC_z, &prod.path), log(1.0));
  eq(imm_path_nsteps(&prod.path), 3U);
  close(imm_path_score(&prod.path), log(1.0));

  eq(imm_eseq_setup(&eseq, &CCC_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(1.0)));

  close(imm_hmm_loglik(&hmm, &CCC_z, &prod.path), log(1.0));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), log(1.0));

  eq(imm_eseq_setup(&eseq, &CCA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.01)));

  close(imm_hmm_loglik(&hmm, &CCA_z, &prod.path), log(0.01));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), log(0.01));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &CCA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.9)));

  close(imm_hmm_loglik(&hmm, &CCA_z, &prod.path), log(0.9));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &CCAB_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (2 * log(0.9)));

  close(imm_hmm_loglik(&hmm, &CCAB_z, &prod.path), 2 * log(0.9));
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), 2 * log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &I0.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &CCAB_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.9 * 0.5 * 0.1)));

  close(imm_hmm_loglik(&hmm, &CCAB_z, &prod.path), log(0.9 * 0.5 * 0.1));
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), log(0.9 * 0.5 * 0.1));

  eq(imm_eseq_setup(&eseq, &CCABB_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.9) + 2 * (log(0.05))));

  close(imm_hmm_loglik(&hmm, &CCABB_z, &prod.path), log(0.9) + 2 * (log(0.05)));
  eq(imm_path_nsteps(&prod.path), 6U);
  close(imm_path_score(&prod.path), log(0.9) + 2 * (log(0.05)));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  float des = log(0.9) + log(0.5) + log(0.1) + log(0.5) + log(0.01);
  eq(imm_eseq_setup(&eseq, &CCABA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &CCABA_z, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 6U);
  close(imm_path_score(&prod.path), des);

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &D1.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &AA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.01) + log(0.9)));

  close(imm_hmm_loglik(&hmm, &AA_z, &prod.path), log(0.01) + log(0.9));
  eq(imm_path_nsteps(&prod.path), 4U);
  close(imm_path_score(&prod.path), log(0.01) + log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &D2.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &AA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.01) + log(0.9)));

  close(imm_hmm_loglik(&hmm, &AA_z, &prod.path), log(0.01) + log(0.9));
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), log(0.01) + log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &E.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  eq(imm_eseq_setup(&eseq, &AA_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, (log(0.01) + log(0.9)));

  close(imm_hmm_loglik(&hmm, &AA_z, &prod.path), log(0.01) + log(0.9));
  eq(imm_path_nsteps(&prod.path), 6U);
  close(imm_path_score(&prod.path), log(0.01) + log(0.9));

  imm_dp_cleanup(&dp);
  imm_hmm_init_dp(&hmm, &M2.super, &dp);
  eq(imm_task_reset(task, &dp), 0);

  des = log(0.01) + log(0.9) + log(0.5);
  eq(imm_eseq_setup(&eseq, &AAB_z), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, des);

  close(imm_hmm_loglik(&hmm, &AAB_z, &prod.path), des);
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), des);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
}

static void cycle_mute_ending(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code_ab);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code_ab);

  struct imm_mute_state start;
  imm_mute_state_init(&start, 0, &abc_ab);
  imm_hmm_add_state(&hmm, &start.super);
  imm_hmm_set_start(&hmm, &start);

  struct imm_mute_state B;
  imm_mute_state_init(&B, 1, &abc_ab);
  imm_hmm_add_state(&hmm, &B.super);

  float lprobs[] = {log(0.01), log(0.01)};
  struct imm_normal_state M;
  imm_normal_state_init(&M, 2, &abc_ab, lprobs);
  imm_hmm_add_state(&hmm, &M.super);

  struct imm_mute_state E;
  imm_mute_state_init(&E, 3, &abc_ab);
  imm_hmm_add_state(&hmm, &E.super);

  struct imm_mute_state J;
  imm_mute_state_init(&J, 4, &abc_ab);
  imm_hmm_add_state(&hmm, &J.super);

  struct imm_mute_state end;
  imm_mute_state_init(&end, 5, &abc_ab);
  imm_hmm_add_state(&hmm, &end.super);

  imm_hmm_set_trans(&hmm, &start.super, &B.super, log(0.1));
  imm_hmm_set_trans(&hmm, &B.super, &M.super, log(0.1));
  imm_hmm_set_trans(&hmm, &M.super, &E.super, log(0.1));
  imm_hmm_set_trans(&hmm, &E.super, &end.super, log(0.1));
  imm_hmm_set_trans(&hmm, &E.super, &J.super, log(0.1));
  imm_hmm_set_trans(&hmm, &J.super, &B.super, log(0.1));

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &end.super, &dp);
  struct imm_task *task = imm_task_new(&dp);

  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -13.815510557964272);

  close(imm_hmm_loglik(&hmm, &A_ab, &prod.path), -13.815510557964272);
  eq(imm_path_nsteps(&prod.path), 5U);
  close(imm_path_score(&prod.path), -13.815510557964272);

  unsigned BM =
      imm_dp_trans_idx(&dp, imm_state_idx(&B.super), imm_state_idx(&M.super));

  imm_dp_change_trans(&dp, BM, log(1.0));

  eq(imm_eseq_setup(&eseq, &A_ab), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  eq(imm_path_nsteps(&prod.path), 5U);
  close(prod.loglik, -11.5129254650);

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

  lrun("one_mute_state", one_mute_state);
  lrun("two_mute_states", two_mute_states);
  lrun("mute_cycle", mute_cycle);
  lrun("one_normal_state", one_normal_state);
  lrun("two_normal_states", two_normal_states);
  lrun("normal_states", normal_states);
  lrun("profile1", profile1);
  lrun("profile2", profile2);
  lrun("profile_delete", profile_delete);
  lrun("global_profile", global_profile);
  lrun("cycle_mute_ending", cycle_mute_ending);

  return lfails != 0;
}
