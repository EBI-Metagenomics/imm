#ifndef CPATH_H
#define CPATH_H

#include "compiler.h"
#include "imm/bitmap.h"
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
struct cpath
{
    unsigned long *bit;
    unsigned nstates;
    uint16_t *state_offset;
    uint8_t *trans_bits;
};

CONST_ATTR TEMPLATE unsigned __path_invalid(unsigned bits)
{
    return (unsigned)((1 << bits) - 1);
}

TEMPLATE unsigned __path_state_bits(struct cpath const *path, unsigned pos,
                                    unsigned state)
{
    (void)pos;
    return (unsigned)(path->state_offset[state + 1] -
                      path->state_offset[state]);
}

TEMPLATE unsigned __path_seqlen_bits(struct cpath const *path, unsigned pos,
                                     unsigned state)
{
    return __path_state_bits(path, pos, state) - path->trans_bits[state];
}

TEMPLATE uint64_t start_bit(struct cpath const *path, unsigned pos,
                            unsigned state)
{
    return (uint64_t)pos * path->state_offset[path->nstates] +
           path->state_offset[state];
}

void imm__cpath_del(struct cpath const *path);

int imm_cpath_init(struct cpath *path, struct imm_state_table const *state_tbl,
                   struct imm_trans_table const *trans_tbl);

int imm__cpath_reset(struct cpath *, struct imm_state_table const *state_tbl,
                     struct imm_trans_table const *trans_tbl);

int imm_cpath_setup(struct cpath *, unsigned len);

TEMPLATE unsigned cpath_seqlen(struct cpath const *path, unsigned pos,
                               unsigned state)
{
    uint64_t start = start_bit(path, pos, state) + path->trans_bits[state];
    return (unsigned)imm_bitmap_get(path->bit, start,
                                    __path_seqlen_bits(path, pos, state));
}

TEMPLATE unsigned cpath_trans(struct cpath const *path, unsigned pos,
                              unsigned state)
{
    uint64_t start = start_bit(path, pos, state);
    return (unsigned)imm_bitmap_get(path->bit, start, path->trans_bits[state]);
}

TEMPLATE bool cpath_valid(struct cpath const *path, unsigned pos,
                          unsigned state)
{
    return !!(__path_invalid(__path_seqlen_bits(path, pos, state)) ^
              cpath_seqlen(path, pos, state));
}

TEMPLATE void cpath_set_seqlen(struct cpath *path, unsigned pos, unsigned state,
                               unsigned len)
{
    uint64_t start = start_bit(path, pos, state) + path->trans_bits[state];
    imm_bitmap_set(path->bit, len, start, __path_seqlen_bits(path, pos, state));
}

TEMPLATE void cpath_set_trans(struct cpath *path, unsigned pos, unsigned state,
                              unsigned trans)
{
    uint64_t start = start_bit(path, pos, state);
    imm_bitmap_set(path->bit, trans, start, path->trans_bits[state]);
}

TEMPLATE void path_invalidate(struct cpath *path, unsigned pos, unsigned state)
{
    unsigned seqlen = __path_invalid(__path_seqlen_bits(path, pos, state));
    cpath_set_seqlen(path, pos, state, seqlen);
}

#endif
