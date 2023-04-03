#include "codon.h"
#include "codon_lprob.h"
#include "codon_marg.h"
#include "codon_state.h"
#include "dna.h"
#include "lprob.h"
#include "seq.h"
#include "test_helper.h"

static void codon_lprob(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ACC");

  ok(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
  imm_codon_lprob_set(&codonp, codon, log(0.5));
  close(imm_codon_lprob_get(&codonp, codon), log(0.5));

  imm_codon_lprob_normalize(&codonp);
  close(imm_codon_lprob_get(&codonp, codon), log(1.0));

  codon = IMM_CODON(nuclt, "ACX");
  ok(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
}

static void codonm_nonmarginal(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), log(0.8));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), log(0.1));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), log(0.8));

  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), log(0.1));

  ok(imm_lprob_is_zero(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "TTT"))));
}

static void codonm_marginal(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), log(0.8));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), log(0.1));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), log(0.8));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), log(0.1));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATX")), log(0.9));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "AXX")), log(0.9));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXX")), log(0.9));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XTX")), log(0.9));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXG")), log(0.8));
  close(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXT")), log(0.1));
}

static void codon_state(void)
{
  struct imm_nuclt nuclt = {0};
  imm_nuclt_init(&nuclt, "ACGT", 'X');
  struct imm_abc const *abc = &nuclt.super;

  struct imm_codon_lprob codonp = imm_codon_lprob(&nuclt);
  struct imm_codon codon = IMM_CODON(&nuclt, "ATG");

  imm_codon_lprob_set(&codonp, codon, log(0.8 / 0.9));
  codon = IMM_CODON(&nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1 / 0.9));

  struct imm_codon_state state = {0};
  imm_codon_state_init(&state, 0, &codonp);
  const struct imm_state *s = &state.super;

  struct imm_seq seq = imm_seq(IMM_STR("ATG"), abc);
  close(imm_state_lprob(s, &seq), log(0.8 / 0.9));

  seq = imm_seq(IMM_STR("AG"), abc);
  ok(imm_lprob_is_nan(imm_state_lprob(s, &seq)));
}

int main()
{
  lrun("codon_lprob", codon_lprob);
  lrun("codonm_nonmarginal", codonm_nonmarginal);
  lrun("codonm_marginal", codonm_marginal);
  lrun("codon_state", codon_state);
  return lfails != 0;
}
