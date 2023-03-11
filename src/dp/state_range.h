#ifndef DP_STATE_RANGE_H
#define DP_STATE_RANGE_H

#include <assert.h>

struct state_range
{
    unsigned state;
    unsigned min_len;
    unsigned max_len;
};

static inline struct state_range state_range(unsigned state, unsigned minlen,
                                             unsigned maxlen)
{
    assert(minlen <= maxlen);
    return (struct state_range){state, minlen, maxlen};
}

#endif
