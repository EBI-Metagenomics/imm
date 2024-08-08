#include "aye.h"
#include "imm_abc.h"
#include "imm_dp.h"
#include "imm_ex1.h"
#include "imm_ex2.h"
#include "imm_hmm.h"
#include "imm_mute_state.h"
#include "imm_normal_state.h"
#include "imm_prod.h"
#include "imm_task.h"
#include "lite_pack_io.h"
#include "near.h"
#include <fcntl.h>
#include <unistd.h>

static struct imm_abc abc;
static struct imm_code code;
static struct imm_seq EMPTY;
static struct imm_seq A;
static struct imm_seq ATT;

static void dp_illegal(void)
{
  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 0, &abc);

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 1, &abc);

  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_dp dp;
  aye(imm_hmm_init_dp(hmm, &dp) == IMM_ENOSTART);
  imm_dp_cleanup(&dp);

  aye(imm_hmm_add_state(hmm, &state.super) == 0);
  aye(imm_hmm_init_dp(hmm, &dp) == IMM_ENOSTART);
  imm_dp_cleanup(&dp);

  aye(imm_hmm_set_start(hmm, &state) == 0);
  aye(imm_hmm_set_end(hmm, &state) == 0);
  aye(imm_hmm_init_dp(hmm, &dp) == 0);
  imm_dp_cleanup(&dp);

  aye(imm_hmm_add_state(hmm, &start.super) == 0);
  aye(imm_hmm_add_state(hmm, &end.super) == 0);
  aye(imm_hmm_set_trans(hmm, &start.super, &state.super, 0) == 0);
  aye(imm_hmm_set_trans(hmm, &state.super, &state.super, logf(0.5f)) == 0);
  aye(imm_hmm_set_trans(hmm, &state.super, &end.super, 0) == 0);
  aye(imm_hmm_set_start(hmm, &start) == 0);
  aye(imm_hmm_set_end(hmm, &end) == 0);
  aye(imm_hmm_init_dp(hmm, &dp) == IMM_EMUTECYLES);
  imm_dp_cleanup(&dp);
  imm_hmm_del(hmm);
}

static void dp_empty_path(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  aye(imm_hmm_add_state(hmm, &state.super) == 0);
  aye(imm_hmm_set_start(hmm, &state) == 0);
  aye(imm_hmm_set_end(hmm, &state) == 0);
  struct imm_dp dp;
  aye(imm_hmm_init_dp(hmm, &dp) == 0);

  struct imm_task *task = imm_task_new(&dp);
  aye(imm_eseq_setup(&eseq, &A) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_path_nsteps(&prod.path) == 0);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_hmm_del(hmm);
}

static void dp_one_mute(void)
{
  struct imm_mute_state state;
  imm_mute_state_init(&state, 3, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  aye(imm_hmm_add_state(hmm, &state.super) == 0);

  aye(imm_hmm_set_start(hmm, &state) == 0);
  aye(imm_hmm_set_end(hmm, &state) == 0);
  struct imm_dp dp;
  aye(imm_hmm_init_dp(hmm, &dp) == 0);

  struct imm_task *task = imm_task_new(&dp);
  aye(imm_dp_viterbi(&dp, task, &prod) == IMM_ENOSEQ);

  aye(imm_eseq_setup(&eseq, &EMPTY) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_path_nsteps(&prod.path) == 1);

  aye(imm_eseq_setup(&eseq, &ATT) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_path_nsteps(&prod.path) == 0);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_hmm_del(hmm);
}

static void dp_two_mutes(void)
{
  struct imm_mute_state state0;
  imm_mute_state_init(&state0, 0, &abc);
  struct imm_mute_state state1;
  imm_mute_state_init(&state1, 12, &abc);
  struct imm_hmm *hmm = imm_hmm_new(&code);
  struct imm_prod prod = imm_prod();

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  aye(imm_hmm_add_state(hmm, &state0.super) == 0);
  aye(imm_hmm_add_state(hmm, &state1.super) == 0);

  aye(imm_hmm_set_trans(hmm, &state0.super, &state1.super, logf(0.5f)) == 0);

  struct imm_dp dp;

  aye(imm_hmm_init_dp(hmm, &dp) == IMM_ENOSTART);
  aye(imm_hmm_set_start(hmm, &state0) == 0);

  aye(imm_hmm_init_dp(hmm, &dp) == IMM_ENOEND);
  aye(imm_hmm_set_end(hmm, &state1) == 0);

  aye(imm_hmm_init_dp(hmm, &dp) == 0);

  struct imm_task *task = imm_task_new(&dp);

  aye(imm_dp_viterbi(&dp, task, &prod) == IMM_ENOSEQ);

  aye(imm_eseq_setup(&eseq, &EMPTY) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_path_nsteps(&prod.path) == 2);
  aye(imm_path_step(&prod.path, 0)->seqsize == 0);
  aye(imm_path_step(&prod.path, 0)->state_id == 0);
  aye(imm_path_step(&prod.path, 1)->seqsize == 0);
  aye(imm_path_step(&prod.path, 1)->state_id == 12);

  aye(imm_eseq_setup(&eseq, &ATT) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_path_nsteps(&prod.path) == 0);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  imm_task_del(task);
  imm_hmm_del(hmm);
}

static void dp_io_example1(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};

  imm_ex1_init(10);
  struct imm_ex1 *m = &imm_ex1;
  struct imm_dp dp;
  aye(imm_hmm_init_dp(imm_ex1.hmm, &dp) == 0);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  int fd = open("dp_example1.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_dp_pack(&dp, &writer) == 0);
  aye(lio_wrelease(&writer) == fd);
  aye(close(fd) == 0);
  imm_dp_cleanup(&dp);

  imm_dp_init(&dp, &m->code);
  fd = open("dp_example1.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_dp_unpack(&dp, &reader) == 0);
  aye(lio_rrelease(&reader) == fd);
  aye(close(fd) == 0);

  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq_unsafe(imm_str("BMMMEJBMMME"), &m->abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, -41.845375f);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);
  remove("dp_example1.imm");
}

static void dp_io_example2(void)
{
  struct lio_writer writer = {0};
  struct lio_reader reader = {0};

  imm_ex2_init(10, imm_span(1, 5));
  struct imm_ex2 *m = &imm_ex2;
  struct imm_dp dp;
  imm_hmm_init_dp(m->hmm, &dp);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &m->code);

  struct imm_abc const *dna_abc = &m->dna->super.super;
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();
  struct imm_seq seq = imm_seq_unsafe(imm_str(imm_ex2_seq), dna_abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, 41.929977f);
  imm_prod_cleanup(&prod);
  imm_task_del(task);

  int fd = open("dp_frame.imm", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  aye(fd != 0);
  lio_wsetup(&writer, fd);
  aye(imm_dp_pack(&dp, &writer) == 0);
  aye(lio_wrelease(&writer) == fd);
  aye(close(fd) == 0);
  imm_dp_cleanup(&dp);

  imm_dp_init(&dp, &m->code);
  fd = open("dp_frame.imm", O_RDONLY, 0644);
  aye(fd != 0);
  lio_rsetup(&reader, fd);
  aye(imm_dp_unpack(&dp, &reader) == 0);
  aye(lio_rrelease(&reader) == fd);
  aye(close(fd) == 0);

  task = imm_task_new(&dp);
  prod = imm_prod();
  seq = imm_seq_unsafe(imm_str(imm_ex2_seq), dna_abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  near(prod.loglik, 41.929977f);

  imm_eseq_cleanup(&eseq);
  imm_prod_cleanup(&prod);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_ex2_cleanup();
  remove("dp_frame.imm");
}

int main(void)
{
  aye_begin();
  imm_abc_init(&abc, imm_str("ACGT"), '*');
  imm_code_init(&code, &abc);
  EMPTY = imm_seq_unsafe(imm_str(""), &abc);
  A = imm_seq_unsafe(imm_str("A"), &abc);
  ATT = imm_seq_unsafe(imm_str("ATT"), &abc);

  dp_illegal();
  dp_empty_path();
  dp_one_mute();
  dp_two_mutes();
  dp_io_example1();
  dp_io_example2();

  return aye_end();
}
