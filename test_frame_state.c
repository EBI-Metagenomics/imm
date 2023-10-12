#include "cartes.h"
#include "codon_lprob.h"
#include "dna.h"
#include "frame_state.h"
#include "lprob.h"
#include "span.h"
#include "vendor/minctest.h"

static void run_frame_state1(float lprobs[], struct imm_span span)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp = imm_nuclt_lprob(
      nuclt, (float[]){log(0.25), log(0.25), log(0.25), log(0.25)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, log(0.8 / 0.9));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1 / 0.9));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, span);
  struct imm_state *s = &state.super;

  struct imm_seq seq = imm_seq_unsafe(imm_str("A"), abc);
  close(imm_state_lprob(s, &seq), lprobs[0]);
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  close(imm_state_lprob(s, &seq), lprobs[1]);
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  close(imm_state_lprob(s, &seq), lprobs[2]);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  close(imm_state_lprob(s, &seq), lprobs[3]);
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  close(imm_state_lprob(s, &seq), lprobs[4]);
  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  close(imm_state_lprob(s, &seq), lprobs[5]);
  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  close(imm_state_lprob(s, &seq), lprobs[6]);
}

static void frame_state1(void)
{
  float lprobs_15[] = {-5.9145034795, -2.9158434138, -6.9055970891,
                       -0.5347732947, -2.5902373362, -6.8810321984,
                       -12.0882895834};
  run_frame_state1(lprobs_15, imm_span(1, 5));
}

static void frame_state2(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp =
      imm_nuclt_lprob(nuclt, (float[]){log(0.1), log(0.2), log(0.3), log(0.4)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);
  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, log(0.8 / 0.9));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1 / 0.9));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));
  struct imm_state *s = &state.super;

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("A"), abc);
  close(imm_state_lprob(s, &seq), -5.9145034795);
  seq = imm_seq_unsafe(imm_str("C"), abc);
  ok(imm_lprob_is_zero(imm_state_lprob(s, &seq)));
  seq = imm_seq_unsafe(imm_str("G"), abc);
  close(imm_state_lprob(s, &seq), -6.0322865151);
  seq = imm_seq_unsafe(imm_str("T"), abc);
  close(imm_state_lprob(s, &seq), -5.8091429638);

  seq = imm_seq_unsafe(imm_str("AT"), abc);
  close(imm_state_lprob(s, &seq), -2.9159357400);
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  close(imm_state_lprob(s, &seq), -7.8215183173);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  close(imm_state_lprob(s, &seq), -0.5344319144);
  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  close(imm_state_lprob(s, &seq), -7.1294800576);
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  close(imm_state_lprob(s, &seq), -2.5751452135);

  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  close(imm_state_lprob(s, &seq), -7.7896445735);
  seq = imm_seq_unsafe(imm_str("ACTG"), abc);
  close(imm_state_lprob(s, &seq), -5.0366370866);

  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  close(imm_state_lprob(s, &seq), -13.9208710471);
}

static void frame_state3(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp =
      imm_nuclt_lprob(nuclt, (float[]){log(0.1), log(0.2), log(0.3), log(0.4)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, log(0.8) - log(1.3));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1) - log(1.3));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, log(0.4) - log(1.3));
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));
  struct imm_state *s = &state.super;

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("A"), abc);
  close(imm_state_lprob(s, &seq), -6.2822282596);
  seq = imm_seq_unsafe(imm_str("C"), abc);
  close(imm_state_lprob(s, &seq), -7.0931584758);
  seq = imm_seq_unsafe(imm_str("G"), abc);
  close(imm_state_lprob(s, &seq), -5.9945461872);
  seq = imm_seq_unsafe(imm_str("T"), abc);
  close(imm_state_lprob(s, &seq), -5.8403955073);
  seq = imm_seq_unsafe(imm_str("AT"), abc);
  close(imm_state_lprob(s, &seq), -3.2834143360);
  seq = imm_seq_unsafe(imm_str("CG"), abc);
  close(imm_state_lprob(s, &seq), -9.3957435523);
  seq = imm_seq_unsafe(imm_str("ATA"), abc);
  close(imm_state_lprob(s, &seq), -8.1891199599);
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  close(imm_state_lprob(s, &seq), -0.9021561046);
  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  close(imm_state_lprob(s, &seq), -2.9428648052);
  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  close(imm_state_lprob(s, &seq), -7.3148113691);
  seq = imm_seq_unsafe(imm_str("GTC"), abc);
  close(imm_state_lprob(s, &seq), -1.5951613416);
  seq = imm_seq_unsafe(imm_str("ATTA"), abc);
  close(imm_state_lprob(s, &seq), -8.1573693536);
  seq = imm_seq_unsafe(imm_str("GTTC"), abc);
  close(imm_state_lprob(s, &seq), -4.7116424205);
  seq = imm_seq_unsafe(imm_str("ACTG"), abc);
  close(imm_state_lprob(s, &seq), -5.4043618667);
  seq = imm_seq_unsafe(imm_str("ATTAA"), abc);
  close(imm_state_lprob(s, &seq), -14.2885958273);
  seq = imm_seq_unsafe(imm_str("GTCAA"), abc);
  close(imm_state_lprob(s, &seq), -12.9023014661);
}

static void frame_state_lposterior(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp =
      imm_nuclt_lprob(nuclt, (float[]){log(0.1), log(0.2), log(0.3), log(0.4)});

  char const *symbols = imm_abc_symbols(abc);
  unsigned length = imm_abc_size(abc);

  struct imm_cartes codon_iter;
  imm_cartes_init(&codon_iter, symbols, length, 3);
  char const *codon_item = NULL;

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  imm_cartes_setup(&codon_iter, 3);
  while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
  {
    struct imm_codon codon = IMM_CODON(nuclt, codon_item);
    imm_codon_lprob_set(&codonp, codon, log(0.001));
  }
  imm_cartes_cleanup(&codon_iter);

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, log(0.8));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, log(0.4));

  imm_codon_lprob_normalize(&codonp);
  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  imm_cartes_init(&codon_iter, symbols, length, 3);
  imm_cartes_setup(&codon_iter, 3);

  while ((codon_item = imm_cartes_next(&codon_iter)) != NULL)
  {
    codon = IMM_CODON(nuclt, codon_item);

    float total = imm_lprob_zero();
    for (uint16_t times = 1; times < 6; ++times)
    {
      struct imm_cartes seq_iter;
      imm_cartes_init(&seq_iter, symbols, length, times);
      imm_cartes_setup(&seq_iter, times);
      char const *seq = NULL;

      while ((seq = imm_cartes_next(&seq_iter)) != NULL)
      {
        struct imm_seq tmp = imm_seq_unsafe((struct imm_str){times, seq}, abc);
        float lprob = imm_frame_state_lposterior(&state, &codon, &tmp);
        lprob -= imm_codon_marg_lprob(&codonm, codon);
        total = imm_lprob_add(total, lprob);
      }
      imm_cartes_cleanup(&seq_iter);
    }
    close((float)exp(total), 1.0);
  }
  imm_cartes_cleanup(&codon_iter);
}

static void frame_state_decode(void)
{
  struct imm_dna const *dna = &imm_dna_iupac;
  struct imm_nuclt const *nuclt = &dna->super;
  struct imm_abc const *abc = &nuclt->super;
  struct imm_nuclt_lprob nucltp =
      imm_nuclt_lprob(nuclt, (float[]){log(0.1), log(0.2), log(0.3), log(0.4)});

  struct imm_codon_lprob codonp = imm_codon_lprob(nuclt);

  struct imm_codon codon = IMM_CODON(nuclt, "ATG");
  imm_codon_lprob_set(&codonp, codon, log(0.8) - log(1.3));
  codon = IMM_CODON(nuclt, "ATT");
  imm_codon_lprob_set(&codonp, codon, log(0.1) - log(1.3));
  codon = IMM_CODON(nuclt, "GTC");
  imm_codon_lprob_set(&codonp, codon, log(0.4) - log(1.3));

  struct imm_codon_marg codonm = imm_codon_marg(&codonp);

  struct imm_frame_state state;
  imm_frame_state_init(&state, 0, &nucltp, &codonm, 0.1f, imm_span(1, 5));

  struct imm_seq seq;
  seq = imm_seq_unsafe(imm_str("ATG"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -0.9025667126);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGT"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -4.7105990700);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGA"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -6.0977143369);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATGGT"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -9.0311004552);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("ATT"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -2.9771014466);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'T'));

  seq = imm_seq_unsafe(imm_str("ATC"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -7.7202251148);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("TC"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -4.1990898725);
  ok(codon.a == imm_abc_symbol_idx(abc, 'G') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'C'));

  seq = imm_seq_unsafe(imm_str("A"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -6.4000112953);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("AG"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -3.5071734613);
  ok(codon.a == imm_abc_symbol_idx(abc, 'A') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'G'));

  seq = imm_seq_unsafe(imm_str("GC"), abc);
  close(imm_frame_state_decode(&state, &seq, &codon), -4.1997050678);
  ok(codon.a == imm_abc_symbol_idx(abc, 'G') &&
     codon.b == imm_abc_symbol_idx(abc, 'T') &&
     codon.c == imm_abc_symbol_idx(abc, 'C'));
}

int main(void)
{
  lrun("frame_state1", frame_state1);
  lrun("frame_state2", frame_state2);
  lrun("frame_state3", frame_state3);
  lrun("frame_state_lposterior", frame_state_lposterior);
  lrun("frame_state_decode", frame_state_decode);
  return lfails != 0;
}
