#ifndef IMM_CPATH_H
#define IMM_CPATH_H

#include "bitmap.h"
#include "compiler.h"
#include "state.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct imm_state_table;
struct imm_trans_table;

/*
 * It compactly encodes the optimal step to every `(pos, state)`, where
 * `pos` is a sequence position (meaning `pos` sequence symbols have been
 * consumed by the path so far) and `state` is the landing state of the
 * corresponding step.
 *
 * The optimal step consists in emission length and transition, and
 * is encoded as follows.
 *
 * Encoding
 * --------
 *
 * The data of an optimal step `(pos, state)` starts at bit (inclusive)
 *
 *   start = pos * state_offset[nstates] + state_offset[state];
 *
 * and ends at bit (exclusive)
 *
 *   end = start + size
 *
 * for
 *
 *   size = state_offset[state + 1] - state_offset[state]
 *
 * The transition is the unsigned integer stored in the half-open interval
 *
 *   [start, start + trans_bits[state])
 *
 * and the emission length in the half-open interval
 *
 *   [start + trans_bits[state], end - trans_bits[state])
 */
struct imm_cpath
{
  unsigned nstates;
  uint16_t ncols;
  unsigned long *bit;
  uint16_t *state_offset;
  uint8_t *trans_bits;
  float *score;
};

imm_const_template unsigned imm_cpath_invalid(unsigned bits)
{
  return (unsigned)((1 << bits) - 1);
}

imm_pure_template unsigned imm_cpath_state_bits(struct imm_cpath const *x,
                                                unsigned state)
{
  return (unsigned)(x->state_offset[state + 1] - x->state_offset[state]);
}

imm_pure_template unsigned imm_cpath_seqlen_bits(struct imm_cpath const *x,
                                                 unsigned state)
{
  return imm_cpath_state_bits(x, state) - x->trans_bits[state];
}

imm_pure_template unsigned long
imm_cpath_start_bit(struct imm_cpath const *x, unsigned pos, unsigned state)
{
  return (unsigned long)pos * x->ncols + x->state_offset[state];
}

void imm_cpath_cleanup(struct imm_cpath *);

int imm_cpath_init(struct imm_cpath *, struct imm_state_table const *,
                   struct imm_trans_table const *);

int imm_cpath_reset(struct imm_cpath *, struct imm_state_table const *,
                    struct imm_trans_table const *);

int imm_cpath_setup(struct imm_cpath *, unsigned len);

imm_pure_template unsigned imm_cpath_seqlen_idx(struct imm_cpath const *x,
                                                unsigned pos, unsigned state)
{
  unsigned long start =
      imm_cpath_start_bit(x, pos, state) + x->trans_bits[state];
  return (unsigned)imm_bitmap_get(x->bit, start,
                                  imm_cpath_seqlen_bits(x, state));
}

imm_pure_template unsigned imm_cpath_trans(struct imm_cpath const *x,
                                           unsigned pos, unsigned state)
{
  unsigned long start = imm_cpath_start_bit(x, pos, state);
  return (unsigned)imm_bitmap_get(x->bit, start, x->trans_bits[state]);
}

imm_pure_template bool imm_cpath_valid(struct imm_cpath const *x, unsigned pos,
                                       unsigned state)
{
  return !!(imm_cpath_invalid(imm_cpath_seqlen_bits(x, state)) ^
            imm_cpath_seqlen_idx(x, pos, state));
}

imm_template void imm_cpath_set_seqlen_idx(struct imm_cpath *x, unsigned pos,
                                           unsigned state, unsigned len)
{
  unsigned long start =
      imm_cpath_start_bit(x, pos, state) + x->trans_bits[state];
  imm_bitmap_set(x->bit, len, start, imm_cpath_seqlen_bits(x, state));
}

imm_template void imm_cpath_set_trans(struct imm_cpath *x, unsigned pos,
                                      unsigned state, unsigned trans)
{
  unsigned long start = imm_cpath_start_bit(x, pos, state);
  imm_bitmap_set(x->bit, trans, start, x->trans_bits[state]);
}

imm_template void imm_cpath_invalidate(struct imm_cpath *x, unsigned pos,
                                       unsigned state)
{
  unsigned seqlen = imm_cpath_invalid(imm_cpath_seqlen_bits(x, state));
  imm_cpath_set_seqlen_idx(x, pos, state, seqlen);
}

#endif
