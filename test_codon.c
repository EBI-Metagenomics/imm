#include "imm_codon.h"
#include "imm_codon_lprob.h"
#include "imm_codon_marg.h"
#include "imm_codon_state.h"
#include "imm_dna.h"
#include "imm_lprob.h"
#include "imm_minctest.h"
#include "imm_seq.h"

static void codon_lprob(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ACC");

  ok(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
  imm_codon_lprob_set(&codonp, codon, logf(0.5f));
  near(imm_codon_lprob_get(&codonp, codon), logf(0.5f));

  imm_codon_lprob_normalize(&codonp);
  near(imm_codon_lprob_get(&codonp, codon), logf(1.0f));

  codon = IMM_CODON(nuclt, "ACX");
  ok(imm_lprob_is_zero(imm_codon_lprob_get(&codonp, codon)));
}

static void codonm_nonmarginal(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), logf(0.8f));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), logf(0.1f));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), logf(0.8f));

  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), logf(0.1f));

  ok(imm_lprob_is_zero(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "TTT"))));
}

static void codonm_marginal(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATG"), logf(0.8f));
  imm_codon_lprob_set(&codonp, IMM_CODON(nuclt, "ATT"), logf(0.1f));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATG")), logf(0.8f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATT")), logf(0.1f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "ATX")), logf(0.9f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "AXX")), logf(0.9f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXX")), logf(0.9f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XTX")), logf(0.9f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXG")), logf(0.8f));
  near(imm_codon_marg_lprob(&codonm, IMM_CODON(nuclt, "XXT")), logf(0.1f));
}

static void codon_state(void)
{
  struct imm_nuclt nuclt = {0};
  imm_nuclt_init(&nuclt, "ACGT", 'X');
  struct imm_abc const *abc = &nuclt.super;

  struct imm_codon_lprob codonp = imm_codon_lprob(&nuclt);
  struct imm_codon codon = IMM_CODON(&nuclt, "ATG");

  imm_codon_lprob_set(&codonp, codon, logf(0.8f / 0.9f));
  codon = IMM_CODON(&nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f / 0.9f));

  struct imm_codon_state state = {0};
  imm_codon_state_init(&state, 0, &codonp);
  const struct imm_state *s = &state.super;

  struct imm_seq seq = imm_seq_unsafe(imm_str("ATG"), abc);
  near(imm_state_lprob(s, &seq), logf(0.8f / 0.9f));

  seq = imm_seq_unsafe(imm_str("AG"), abc);
  ok(imm_lprob_is_nan(imm_state_lprob(s, &seq)));
}

int main(void)
{
  lrun("codon_lprob", codon_lprob);
  lrun("codonm_nonmarginal", codonm_nonmarginal);
  lrun("codonm_marginal", codonm_marginal);
  lrun("codon_state", codon_state);
  return lfails != 0;
}
