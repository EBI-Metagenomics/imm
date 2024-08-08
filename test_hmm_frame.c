#include "aye.h"
#include "imm_codon_lprob.h"
#include "imm_dna.h"
#include "imm_dp.h"
#include "imm_frame_state.h"
#include "imm_hmm.h"
#include "imm_lprob.h"
#include "imm_mute_state.h"
#include "imm_path.h"
#include "imm_prod.h"
#include "imm_task.h"
#include "near.h"

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
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -2.3025850930f));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -0.2231435513142097f));
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  aye(imm_lprob_is_nan(imm_hmm_loglik(hmm, &seq, &path)));

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
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -6.0198639951f));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 1, 0));
  seq = imm_seq_unsafe(imm_str("C"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -7.1184762838f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("A"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -6.0198639951f));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("C"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -7.1184762838f));

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
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -8.9102357365f));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("TG"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -3.2434246877f));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("CC"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -4.2250228758f));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq_unsafe(imm_str("TT"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -5.3267168311f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("AA"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -8.9102357365f));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("TG"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -3.2434246877f));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("CC"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -4.2250228758f));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("TT"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -5.3267168311f));

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
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -7.0123444607f));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -0.6397933781f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -7.0123444607f));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -0.6397933781f));

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
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -11.9829290512f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ATCC"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -11.9829290512f));

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
  aye(imm_lprob_is_zero(imm_hmm_loglik(hmm, &seq, &path)));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(start.super.id, 0, 0));
  imm_path_add(&path, imm_step(state.super.id, 5, 0));
  seq = imm_seq_unsafe(imm_str("ACTAG"), abc);
  aye(near(imm_hmm_loglik(hmm, &seq, &path), -10.1142085574f));

  struct imm_dp dp;
  imm_hmm_init_dp(hmm, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq_unsafe(imm_str("ACGTA"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(imm_lprob_is_nan(prod.loglik));

  imm_hmm_reset_dp(hmm, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq_unsafe(imm_str("ACTAG"), abc);
  aye(imm_eseq_setup(&eseq, &seq) == 0);
  aye(imm_task_setup(task, &eseq) == 0);
  aye(imm_dp_viterbi(&dp, task, &prod) == 0);
  aye(near(prod.loglik, -10.1142085574f));

  imm_eseq_cleanup(&eseq);
  imm_task_del(task);
  imm_dp_cleanup(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
  imm_hmm_del(hmm);
}

int main(void)
{
  aye_begin();
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

  hmm_frame_state_0eps();
  hmm_frame_state_len1();
  hmm_frame_state_len2();
  hmm_frame_state_len3();
  hmm_frame_state_len4();
  hmm_frame_state_len5();

  return aye_end();
}
