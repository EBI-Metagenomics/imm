#include "codon_lprob.h"
#include "dna.h"
#include "dp.h"
#include "frame_state.h"
#include "hmm.h"
#include "lprob.h"
#include "path.h"
#include "prod.h"
#include "task.h"
#include "vendor/minctest.h"

struct imm_nuclt const *nuclt;
struct imm_abc const *abc;
static struct imm_code code;
struct imm_nuclt_lprob nucltp;
struct imm_codon_lprob codonp;
struct imm_codon_marg codonm;

static void hmm_frame_state_0eps(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.0f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  struct imm_seq seq = imm_seq(IMM_STR("ATT"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -2.3025850930);
  seq = imm_seq(IMM_STR("ATG"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -0.2231435513142097);
  seq = imm_seq(IMM_STR("AT"), abc);
  ok(imm_lprob_is_nan(imm_hmm_loglik(&hmm, &seq, &path)));

  imm_path_cleanup(&path);
}

static void hmm_frame_state_len1(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 1, 0));
  struct imm_seq seq = imm_seq(IMM_STR("A"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -6.0198639951);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 1, 0));
  seq = imm_seq(IMM_STR("C"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -7.1184762838);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq(IMM_STR("A"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -6.0198639951);

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("C"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -7.1184762838);

  imm_task_del(task);
  imm_path_cleanup(&path);
  imm_prod_cleanup(&prod);
  imm_dp_del(&dp);
}

static void hmm_frame_state_len2(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  struct imm_seq seq = imm_seq(IMM_STR("AA"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -8.9102357365);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq(IMM_STR("TG"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -3.2434246877);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq(IMM_STR("CC"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -4.2250228758);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 2, 0));
  seq = imm_seq(IMM_STR("TT"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -5.3267168311);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq(IMM_STR("AA"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -8.9102357365);

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("TG"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -3.2434246877);

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("CC"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -4.2250228758);

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("TT"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -5.3267168311);

  imm_task_del(task);
  imm_dp_del(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
}

static void hmm_frame_state_len3(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  struct imm_seq seq = imm_seq(IMM_STR("ATC"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -7.0123444607);

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 3, 0));
  seq = imm_seq(IMM_STR("ATG"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -0.6397933781);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq(IMM_STR("ATC"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -7.0123444607);

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("ATG"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -0.6397933781);

  imm_task_del(task);
  imm_dp_del(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
}

static void hmm_frame_state_len4(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 4, 0));
  struct imm_seq seq = imm_seq(IMM_STR("ATCC"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -11.9829290512);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq(IMM_STR("ATCC"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -11.9829290512);

  imm_task_del(task);
  imm_dp_del(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
}

static void hmm_frame_state_len5(void)
{
  struct imm_hmm hmm;
  imm_hmm_init(&hmm, &code);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f,
                       (struct imm_span){1, 5});

  imm_hmm_add_state(&hmm, &state.super);
  imm_hmm_set_start(&hmm, &state.super, log(1.0));

  struct imm_path path = imm_path();
  imm_path_add(&path, imm_step(state.super.id, 5, 0));
  struct imm_seq seq = imm_seq(IMM_STR("ACGTA"), abc);
  ok(imm_lprob_is_zero(imm_hmm_loglik(&hmm, &seq, &path)));

  imm_path_reset(&path);
  imm_path_add(&path, imm_step(state.super.id, 5, 0));
  seq = imm_seq(IMM_STR("ACTAG"), abc);
  close(imm_hmm_loglik(&hmm, &seq, &path), -10.1142085574);

  struct imm_dp dp;
  imm_hmm_init_dp(&hmm, &state.super, &dp);
  struct imm_task *task = imm_task_new(&dp);
  struct imm_prod prod = imm_prod();

  seq = imm_seq(IMM_STR("ACGTA"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  ok(imm_lprob_is_nan(prod.loglik));

  imm_hmm_reset_dp(&hmm, &state.super, &dp);
  imm_task_reset(task, &dp);
  seq = imm_seq(IMM_STR("ACTAG"), abc);
  eq(imm_task_setup(task, &seq), 0);
  eq(imm_dp_viterbi(&dp, task, &prod), 0);
  close(prod.loglik, -10.1142085574);

  imm_task_del(task);
  imm_dp_del(&dp);
  imm_prod_cleanup(&prod);
  imm_path_cleanup(&path);
}

int main(void)
{
  nuclt = &imm_dna_iupac.super;
  abc = &nuclt->super;
  imm_code_init(&code, abc);

  nucltp = imm_nuclt_lprob(
      nuclt, (float[]){log(0.25), log(0.25), log(0.5), imm_lprob_zero()});

  codonp = imm_codon_lprob(nuclt);
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), log(0.8));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), log(0.1));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "CCC"), log(0.1));

  codonm = imm_codon_marg(&codonp);

  lrun("hmm_frame_state_0eps", hmm_frame_state_0eps);
  lrun("hmm_frame_state_len1", hmm_frame_state_len1);
  lrun("hmm_frame_state_len2", hmm_frame_state_len2);
  lrun("hmm_frame_state_len3", hmm_frame_state_len3);
  lrun("hmm_frame_state_len4", hmm_frame_state_len4);
  lrun("hmm_frame_state_len5", hmm_frame_state_len5);

  return lfails != 0;
}
