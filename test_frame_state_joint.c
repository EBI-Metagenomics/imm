#include "cartes.h"
#include "codon_lprob.h"
#include "codon_marg.h"
#include "dna.h"
#include "frame_state.h"
#include "lprob.h"
#include "nuclt_lprob.h"
#include "state.h"
#include "vendor/minctest.h"

static void setup_codonp(struct imm_codon_lprob *codonp);
static struct imm_nuclt_lprob setup_nucltp(struct imm_nuclt const *);
static void check_joint(struct imm_state const *, struct imm_span);

static void test_frame_state_joint(struct imm_nuclt_lprob const *nucltp,
                                   struct imm_codon_marg const *codonm,
                                   struct imm_span span);

static void test_frame_state_joint(struct imm_nuclt_lprob const *nucltp,
                                   struct imm_codon_marg const *codonm,
                                   struct imm_span span)
{
  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, nucltp, codonm, 0.1f, span);
  check_joint(&state.super, span);
}

static void setup_codonp(struct imm_codon_lprob *codonp)
{
  struct imm_nuclt const *nuclt = codonp->nuclt;
  struct imm_abc const *abc = &nuclt->super;
  char const *symbols = imm_abc_symbols(abc);
  int length = imm_abc_size(abc);

  struct imm_cartes iter;
  imm_cartes_init(&iter, symbols, length);
  char const *item = NULL;

  imm_cartes_setup(&iter, 3);
  while ((item = imm_cartes_next(&iter)) != NULL)
  {
    struct imm_codon codon = IMM_CODON(nuclt, item);
    imm_codon_lprob_set(codonp, codon, logf(0.001f));
  }

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(codonp, codon, logf(0.8f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(codonp, codon, logf(0.1f));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(codonp, codon, logf(0.4f));

  imm_codon_lprob_normalize(codonp);
}

static struct imm_nuclt_lprob setup_nucltp(struct imm_nuclt const *nuclt)
{
  float arr[] = {logf(0.1f), logf(0.2f), logf(0.3f), logf(0.4f)};
  return imm_nuclt_lprob(nuclt, arr);
}

static void check_joint(struct imm_state const *state, struct imm_span span)
{
  struct imm_abc const *abc = imm_state_abc(state);
  char const *symbols = imm_abc_symbols(abc);
  int length = imm_abc_size(abc);

  struct imm_cartes iter;
  imm_cartes_init(&iter, symbols, length);
  float total = imm_lprob_zero();
  for (int times = span.min; times <= span.max; ++times)
  {
    imm_cartes_setup(&iter, times);

    char const *seq = NULL;
    while ((seq = imm_cartes_next(&iter)) != NULL)
    {
      struct imm_seq tmp = imm_seq_unsafe((struct imm_str){times, seq}, abc);
      total = imm_lprob_add(total, imm_state_lprob(state, &tmp));
    }
  }
  near(exp(total), 1.0);
}

static void frame_state_joint15(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  setup_codonp(&codonp);

  struct imm_nuclt_lprob nucltp = setup_nucltp(nuclt);
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  test_frame_state_joint(&nucltp, &codonm, imm_span(1, 5));
}

static void frame_state_joint24(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  setup_codonp(&codonp);

  struct imm_nuclt_lprob nucltp = setup_nucltp(nuclt);
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  test_frame_state_joint(&nucltp, &codonm, imm_span(2, 4));
}

static void frame_state_joint33(void)
{
  struct imm_nuclt const *nuclt = &imm_dna_iupac.super;
  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  setup_codonp(&codonp);

  struct imm_nuclt_lprob nucltp = setup_nucltp(nuclt);
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  test_frame_state_joint(&nucltp, &codonm, imm_span(3, 3));
}

int main(void)
{
  lrun("frame_state_joint15", frame_state_joint15);
  lrun("frame_state_joint24", frame_state_joint24);
  lrun("frame_state_joint33", frame_state_joint33);
  return lfails != 0;
}
