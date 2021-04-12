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
    uint_fast16_t  nstates;
    uint16_t*      bits_state;
    uint8_t*       bits_trans;
};

void cpath_deinit(struct cpath const* path);
void cpath_init(struct cpath* path, struct dp_state_table const* state_tbl, struct dp_trans_table const* trans_tbl);
static inline bool          cpath_valid(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state);
static inline uint_fast8_t  cpath_get_len(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state);
static inline uint_fast16_t cpath_get_trans(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state);
static inline void          cpath_set_invalid(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state);
static inline void cpath_set_len(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state, uint_fast8_t len);
static inline void cpath_set_trans(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state, uint_fast16_t trans);
void               cpath_setup(struct cpath* path, uint_fast32_t len);

static inline uint8_t       __bits_len(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state);
static inline uint_fast64_t __bits_start(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state);

static inline bool cpath_valid(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state)
{
    static unsigned long const invalids[] = {0, 1, 3, 5, 7, 15, 31, 63, 127, 255, 511};
    return invalids[__bits_len(cpath, row, state)] != (unsigned long)cpath_get_len(cpath, row, state);
}

static inline uint_fast8_t cpath_get_len(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state)
{
    uint_fast64_t start = __bits_start(cpath, row, state);
    return (uint_fast8_t)bitarr_get(cpath->bitarr, (uint_fast64_t)(start + cpath->bits_trans[state]),
                                    __bits_len(cpath, row, state));
}

static inline uint_fast16_t cpath_get_trans(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state)
{
    uint_fast64_t start = __bits_start(cpath, row, state);
    return (uint_fast16_t)bitarr_get(cpath->bitarr, start, cpath->bits_trans[state]);
}

static inline void cpath_set_invalid(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state)
{
    static unsigned long const invalids[] = {0, 1, 3, 5, 7, 15, 31, 63, 127, 255, 511};
    cpath_set_len(cpath, row, state, (uint_fast8_t)invalids[__bits_len(cpath, row, state)]);
}

static inline void cpath_set_len(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state, uint_fast8_t len)
{
    uint_fast64_t start = __bits_start(cpath, row, state);
    bitarr_set(cpath->bitarr, (unsigned long)len, start + cpath->bits_trans[state], __bits_len(cpath, row, state));
}

static inline void cpath_set_trans(struct cpath* cpath, uint_fast32_t row, uint_fast16_t state, uint_fast16_t trans)
{
    uint_fast64_t start = __bits_start(cpath, row, state);
    bitarr_set(cpath->bitarr, (unsigned long)trans, start, cpath->bits_trans[state]);
}

static inline uint8_t __bits_len(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state)
{
    return (uint8_t)(cpath->bits_state[state + 1] - cpath->bits_state[state] - cpath->bits_trans[state]);
}

static inline uint_fast64_t __bits_start(struct cpath const* cpath, uint_fast32_t row, uint_fast16_t state)
{
    return (uint_fast64_t)row * (uint_fast64_t)cpath->bits_state[cpath->nstates] +
           (uint_fast64_t)cpath->bits_state[state];
}

#endif
