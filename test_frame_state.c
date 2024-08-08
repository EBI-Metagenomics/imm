#include "aye.h"
#include "imm_cartes.h"
#include "imm_codon_lprob.h"
#include "imm_dna.h"
#include "imm_frame_state.h"
#include "imm_lprob.h"
#include "imm_span.h"
#include "near.h"

static void run_frame_state1(float lprobs[], struct imm_span span)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.25f), logf(0.25f), logf(0.25f), logf(0.25f)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, logf(0.8f / 0.9f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f / 0.9f));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, span);
  struct imm_state *s = &state.super;

  struct imm_seq seq = imm_seq_unsafe(imm_str("A"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[0]));
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[1]));
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[2]));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[3]));
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[4]));
  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[5]));
  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  aye(near(imm_state_lprob(s, &seq), lprobs[6]));
}

static void frame_state1(void)
{
  float lprobs_15[] = {-5.9145034795f, -2.9158434138f, -6.9055970891f,
                       -0.5347732947f, -2.5902373362f, -6.8810321984f,
                       -12.0882895834f};
  run_frame_state1(lprobs_15, imm_span(1, 5));
}

static void frame_state2(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.1f), logf(0.2f), logf(0.3f), logf(0.4f)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, logf(0.8f / 0.9f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f / 0.9f));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));
  struct imm_state *s = &state.super;

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("A"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.9145034795f));
  seq = imm_seq_unsafe(imm_str("C"), abc);
  aye(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
  seq = imm_seq_unsafe(imm_str("G"), abc);
  aye(near(imm_state_lprob(s, &seq), -6.0322865151f));
  seq = imm_seq_unsafe(imm_str("T"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.8091429638f));

  seq = imm_seq_unsafe(imm_str("AT"), abc);
  aye(near(imm_state_lprob(s, &seq), -2.9159357400f));
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  aye(near(imm_state_lprob(s, &seq), -7.8215183173f));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_state_lprob(s, &seq), -0.5344319144f));
  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  aye(near(imm_state_lprob(s, &seq), -7.1294800576f));
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  aye(near(imm_state_lprob(s, &seq), -2.5751452135f));

  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  aye(near(imm_state_lprob(s, &seq), -7.7896445735f));
  seq = imm_seq_unsafe(imm_str("ACTG"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.0366370866f));

  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  aye(near(imm_state_lprob(s, &seq), -13.9208710471f));
}

static void frame_state3(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.1f), logf(0.2f), logf(0.3f), logf(0.4f)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, logf(0.8f) - logf(1.3f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f) - logf(1.3f));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, logf(0.4f) - logf(1.3f));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));
  struct imm_state *s = &state.super;

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("A"), abc);
  aye(near(imm_state_lprob(s, &seq), -6.2822282596f));
  seq = imm_seq_unsafe(imm_str("C"), abc);
  aye(near(imm_state_lprob(s, &seq), -7.0931584758f));
  seq = imm_seq_unsafe(imm_str("G"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.9945461872f));
  seq = imm_seq_unsafe(imm_str("T"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.8403955073f));
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  aye(near(imm_state_lprob(s, &seq), -3.2834143360f));
  seq = imm_seq_unsafe(imm_str("CG"), abc);
  aye(near(imm_state_lprob(s, &seq), -9.3957435523f));
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  aye(near(imm_state_lprob(s, &seq), -8.1891199599f));
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_state_lprob(s, &seq), -0.9021561046f));
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  aye(near(imm_state_lprob(s, &seq), -2.9428648052f));
  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  aye(near(imm_state_lprob(s, &seq), -7.3148113691f));
  seq = imm_seq_unsafe(imm_str("GTC"), abc);
  aye(near(imm_state_lprob(s, &seq), -1.5951613416f));
  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  aye(near(imm_state_lprob(s, &seq), -8.1573693536f));
  seq = imm_seq_unsafe(imm_str("GTTC"), abc);
  aye(near(imm_state_lprob(s, &seq), -4.7116424205f));
  seq = imm_seq_unsafe(imm_str("ACTG"), abc);
  aye(near(imm_state_lprob(s, &seq), -5.4043618667f));
  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  aye(near(imm_state_lprob(s, &seq), -14.2885958273f));
  seq = imm_seq_unsafe(imm_str("GTCAA"), abc);
  aye(near(imm_state_lprob(s, &seq), -12.9023014661f));
}

static void frame_state_lposterior(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.1f), logf(0.2f), logf(0.3f), logf(0.4f)});

  char const *symbols = imm_abc_symbols(abc);
  int length = imm_abc_size(abc);

  struct imm_cartes codon_iter;
  imm_cartes_init(&codon_iter, symbols, length);
  char const *codon_item = NULL;

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_cartes_setup(&codon_iter, 3);
  while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
  {
    struct imm_codon codon = IMM_CODON(nuclt, codon_item);
    imm_codon_lprob_set(&codonp, codon, logf(0.001f));
  }

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, logf(0.8f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, logf(0.4f));

  imm_codon_lprob_normalize(&codonp);
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  imm_cartes_init(&codon_iter, symbols, length);
  imm_cartes_setup(&codon_iter, 3);

  while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
  {
    codon = IMM_CODON(nuclt, codon_item);

    float total = imm_lprob_zero();
    for (uint16_t times = 1; times < 6; ++times)
    {
      struct imm_cartes seq_iter;
      imm_cartes_init(&seq_iter, symbols, length);
      imm_cartes_setup(&seq_iter, times);
      char const *seq = NULL;

      while ((seq = imm_cartes_next(&seq_iter)) != NULL)
      {
        struct imm_seq tmp = imm_seq_unsafe((struct imm_str){times, seq}, abc);
        float lprob = imm_frame_state_lposterior(&state, &codon, &tmp);
        lprob -= imm_codon_marg_lprob(&codonm, codon);
        total = imm_lprob_add(total, lprob);
      }
    }
    aye(near(expf(total), 1.0));
  }
}

static void frame_state_decode(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){logf(0.1f), logf(0.2f), logf(0.3f), logf(0.4f)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, logf(0.8f) - logf(1.3f));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, logf(0.1f) - logf(1.3f));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, logf(0.4f) - logf(1.3f));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -0.9025667126f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGT"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -4.7105990700f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGA"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -6.0977143369f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGGT"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -9.0311004552f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -2.9771014466f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'T'));

  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -7.7202251148f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("TC"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -4.1990898725f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'G') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'C'));

  seq = imm_seq_unsafe(imm_str("A"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -6.4000112953f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("AG"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -3.5071734613f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'A') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("GC"), abc);
  aye(near(imm_frame_state_decode(&state, &seq, &codon), -4.1997050678f));
  aye(codon.a == imm_abc_symbol_idx(abc, 'G') &&
      codon.b == imm_abc_symbol_idx(abc, 'T') &&
      codon.c == imm_abc_symbol_idx(abc, 'C'));
}

int main(void)
{
  aye_begin();
  frame_state1();
  frame_state2();
  frame_state3();
  frame_state_lposterior();
  frame_state_decode();
  return aye_end();
}
