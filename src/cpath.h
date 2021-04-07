#ifndef CPATH_H
#define CPATH_H

#include "bitarr.h"
#include <stddef.h>
#include <stdint.h>

struct dp_state_table;
struct dp_trans_table;

struct cpath
{
    unsigned long* bitarr;
    uint16_t       nstates;
    uint16_t*      bits_state;
    uint8_t*       bits_trans;
};

void cpath_deinit(struct cpath const* path);
void cpath_init(struct cpath* path, struct dp_state_table const* state_tbl, struct dp_trans_table const* trans_tbl);
static inline bool          cpath_valid(struct cpath const* cpath, unsigned row, unsigned state);
static inline unsigned long cpath_get_len(struct cpath const* cpath, unsigned row, unsigned state);
static inline unsigned long cpath_get_trans(struct cpath const* cpath, unsigned row, unsigned state);
static inline void          cpath_set_invalid(struct cpath* cpath, unsigned row, unsigned state);
static inline void          cpath_set_len(struct cpath* cpath, unsigned row, unsigned state, unsigned long len);
static inline void          cpath_set_trans(struct cpath* cpath, unsigned row, unsigned state, unsigned long trans);
void                        cpath_setup(struct cpath* path, uint32_t len);

static inline uint8_t __bits_len(struct cpath const* cpath, unsigned row, unsigned state);
static inline size_t  __bits_start(struct cpath const* cpath, unsigned row, unsigned state);

static inline bool cpath_valid(struct cpath const* cpath, unsigned row, unsigned state)
{
    static unsigned long const invalids[] = {0, 1, 3, 5, 7, 15, 31, 63, 127, 255, 511};
    return invalids[__bits_len(cpath, row, state)] != cpath_get_len(cpath, row, state);
}

static inline unsigned long cpath_get_len(struct cpath const* cpath, unsigned row, unsigned state)
{
    size_t start = __bits_start(cpath, row, state);
    return bitarr_get(cpath->bitarr, start + cpath->bits_trans[state], __bits_len(cpath, row, state));
}

static inline unsigned long cpath_get_trans(struct cpath const* cpath, unsigned row, unsigned state)
{
    size_t start = __bits_start(cpath, row, state);
    return bitarr_get(cpath->bitarr, start, cpath->bits_trans[state]);
}

static inline void cpath_set_invalid(struct cpath* cpath, unsigned row, unsigned state)
{
    static unsigned long const invalids[] = {0, 1, 3, 5, 7, 15, 31, 63, 127, 255, 511};
    cpath_set_len(cpath, row, state, invalids[__bits_len(cpath, row, state)]);
}

#include <stdio.h>
static inline void cpath_set_len(struct cpath* cpath, unsigned row, unsigned state, unsigned long len)
{
    size_t start = __bits_start(cpath, row, state);
    bitarr_set(cpath->bitarr, len, start + cpath->bits_trans[state], __bits_len(cpath, row, state));
}

static inline void cpath_set_trans(struct cpath* cpath, unsigned row, unsigned state, unsigned long trans)
{
    size_t start = __bits_start(cpath, row, state);
    bitarr_set(cpath->bitarr, trans, start, cpath->bits_trans[state]);
}

static inline uint8_t __bits_len(struct cpath const* cpath, unsigned row, unsigned state)
{
    return (uint8_t)(cpath->bits_state[state + 1] - cpath->bits_state[state] - cpath->bits_trans[state]);
}

static inline size_t __bits_start(struct cpath const* cpath, unsigned row, unsigned state)
{
    return row * cpath->bits_state[cpath->nstates] + cpath->bits_state[state];
}

#endif
