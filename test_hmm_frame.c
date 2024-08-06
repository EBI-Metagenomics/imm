#include "codon_lprob.h"
#include "dna.h"
#include "dp.h"
#include "frame_state.h"
#include "hmm.h"
#include "lprob.h"
#include "minctest.h"
#include "mute_state.h"
#include "path.h"
#include "prod.h"
#include "task.h"

static struct imm_nuclt const *nuclt;
static struct imm_abc const *abc;
static struct imm_code code;
static struct imm_nuclt_lprob nucltp;
static struct imm_codon_lprob codonp;
static struct imm_codon_marg codonm;

static void hmm_frame_state_0eps(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.0f, imm_span(1, 5));

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_start(hmm, &start);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("ATT"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -2.3025850930);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -0.2231435513142097);
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  ok(imm_lprob_is_nan(imm_hmm_loglik(hmm, &seq, &path)));

  imm_path_cleanup(&path);

  imm_hmm_del(hmm);
}

static void hmm_frame_state_len1(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 2, &nucltp.nuclt->super);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_trans(hmm, &state.super, &end.super, 0);
  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 1, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("A"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -6.0198639951);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 1, 0));
  seq = imm_seq_unsafe(imm_str("C"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -7.1184762838);

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("A"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -6.0198639951);

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("C"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -7.1184762838);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_path_cleanup(&path);
  imm_prod_cleanup(&prod);
  imm_dp_cleanup(&dp);

  imm_hmm_del(hmm);
}

static void hmm_frame_state_len2(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 2, &nucltp.nuclt->super);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_trans(hmm, &state.super, &end.super, 0);
  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("AA"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -8.9102357365);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("TG"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -3.2434246877);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("CC"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -4.2250228758);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("TT"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -5.3267168311);

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("AA"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -8.9102357365);

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("TG"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -3.2434246877);

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("CC"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -4.2250228758);

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("TT"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -5.3267168311);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);

  imm_hmm_del(hmm);
}

static void hmm_frame_state_len3(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 2, &nucltp.nuclt->super);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_trans(hmm, &state.super, &end.super, 0);
  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("ATC"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -7.0123444607);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -0.6397933781);

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -7.0123444607);

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -0.6397933781);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);

  imm_hmm_del(hmm);
}

static void hmm_frame_state_len4(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 2, &nucltp.nuclt->super);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_trans(hmm, &state.super, &end.super, 0);
  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 4, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("ATCC"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -11.9829290512);

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ATCC"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -11.9829290512);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
  imm_hmm_del(hmm);
}

static void hmm_frame_state_len5(void)
{
  struct imm_hmm *hmm = imm_hmm_new(&code);

  struct imm_eseq eseq = {0};
  imm_eseq_init(&eseq, &code);

  struct imm_mute_state start = {0};
  imm_mute_state_init(&start, 1, &nucltp.nuclt->super);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_mute_state end = {0};
  imm_mute_state_init(&end, 2, &nucltp.nuclt->super);

  imm_hmm_add_state(hmm, &start.super);
  imm_hmm_add_state(hmm, &state.super);
  imm_hmm_add_state(hmm, &end.super);
  imm_hmm_set_trans(hmm, &start.super, &state.super, 0);
  imm_hmm_set_trans(hmm, &state.super, &end.super, 0);
  imm_hmm_set_start(hmm, &start);
  imm_hmm_set_end(hmm, &end);

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 5, 0));
  struct imm_seq seq = imm_seq_unsafe(imm_str("ACGTA"), abc);
  ok(imm_lprob_is_zero(imm_hmm_loglik(hmm, &seq, &path)));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 5, 0));
  seq = imm_seq_unsafe(imm_str("ACTAG"), abc);
  near(imm_hmm_loglik(hmm, &seq, &path), -10.1142085574);

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ACGTA"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  ok(imm_lprob_is_nan(prod.loglik));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("ACTAG"), abc);
  eq(imm_eseq_setup(&eseq, &seq), 0);
  eq(imm_task_setup(task, &eseq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  near(prod.loglik, -10.1142085574);

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
  imm_hmm_del(hmm);
}

int main(void)
{
  nuclt = &imm_dna_iupac.super;
  abc = &nuclt->super;
  imm_code_init(&code, abc);

  nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.25), logf(0.25), logf(0.5), imm_lprob_zero()});

  codonp = imm_codon_lprob(nuclt);
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), logf(0.8f));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), logf(0.1f));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CCC"), logf(0.1f));

  codonm = imm_codon_marg(&codonp);

  lrun("hmm_frame_state_0eps", hmm_frame_state_0eps);
  lrun("hmm_frame_state_len1", hmm_frame_state_len1);
  lrun("hmm_frame_state_len2", hmm_frame_state_len2);
  lrun("hmm_frame_state_len3", hmm_frame_state_len3);
  lrun("hmm_frame_state_len4", hmm_frame_state_len4);
  lrun("hmm_frame_state_len5", hmm_frame_state_len5);

  return lfails != 0;
}
