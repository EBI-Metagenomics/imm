#ifndef DP_PATH_H
#define DP_PATH_H

#include "dp/bitarr.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct state_table;
struct trans_table;

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
struct path
{
    unsigned long *bit;
    unsigned nstates;
    uint16_t *state_offset;
    uint8_t *trans_bits;
};

static inline unsigned __path_invalid(unsigned bits) { return (1 << bits) - 1; }

static inline unsigned __path_state_bits(struct path const *path, unsigned pos,
                                         unsigned state)
{
    return path->state_offset[state + 1] - path->state_offset[state];
}

static inline unsigned __path_seqlen_bits(struct path const *path, unsigned pos,
                                          unsigned state)
{
    return __path_state_bits(path, pos, state) - path->trans_bits[state];
}

static inline uint64_t start_bit(struct path const *path, unsigned pos,
                                 unsigned state)
{
    return (uint64_t)pos * path->state_offset[path->nstates] +
           path->state_offset[state];
}

void path_deinit(struct path const *path);

void path_init(struct path *path, struct state_table const *state_tbl,
               struct trans_table const *trans_tbl);

void path_setup(struct path *path, unsigned len);

static inline unsigned path_seqlen(struct path const *path, unsigned pos,
                                   unsigned state)
{
    uint64_t start = start_bit(path, pos, state) + path->trans_bits[state];
    return (unsigned)bitarr_get(path->bit, start,
                                __path_seqlen_bits(path, pos, state));
}

static inline unsigned path_trans(struct path const *path, unsigned pos,
                                  unsigned state)
{
    uint64_t start = start_bit(path, pos, state);
    return (unsigned)bitarr_get(path->bit, start, path->trans_bits[state]);
}

static inline bool path_valid(struct path const *path, unsigned pos,
                              unsigned state)
{
    return !!(__path_invalid(__path_seqlen_bits(path, pos, state)) ^
              path_seqlen(path, pos, state));
}

static inline void path_set_seqlen(struct path *path, unsigned pos,
                                   unsigned state, unsigned len)
{
    uint64_t start = start_bit(path, pos, state) + path->trans_bits[state];
    bitarr_set(path->bit, (unsigned long)len, start,
               __path_seqlen_bits(path, pos, state));
}

static inline void path_set_trans(struct path *path, unsigned pos,
                                  unsigned state, unsigned trans)
{
    uint64_t start = start_bit(path, pos, state);
    bitarr_set(path->bit, (unsigned long)trans, start, path->trans_bits[state]);
}

static inline void path_invalidate(struct path *path, unsigned pos,
                                   unsigned state)
{
    unsigned seqlen = __path_invalid(__path_seqlen_bits(path, pos, state));
    path_set_seqlen(path, pos, state, seqlen);
}

#endif
