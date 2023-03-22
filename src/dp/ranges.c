#include "dp/ranges.h"
#include "imm/state_types.h"
#include <assert.h>

// Define as:
//   lowest row such that r >= max(max_seq) > 0 for every r >= row.
static struct imm_range find_safe_past(unsigned seqlen)
{
    assert(IMM_STATE_MAX_SEQLEN > 0);
    if (seqlen >= IMM_STATE_MAX_SEQLEN)
        return imm_range_init(IMM_STATE_MAX_SEQLEN, seqlen + 1);
    return imm_range_init(seqlen + 1, seqlen + 1);
}

// Define as:
//   greatest row such that seqlen >= max(max_seq) + r for every r <= row
//   and subject to row > 0.
//
static struct imm_range find_safe_future(unsigned seqlen)
{
    if (seqlen >= IMM_STATE_MAX_SEQLEN)
        return imm_range_init(1, seqlen - IMM_STATE_MAX_SEQLEN + 1);
    return imm_range_init(1, 1);
}

void dp_ranges_find(struct ranges *x, unsigned len)
{
    struct imm_range past = find_safe_past(len);
    struct imm_range future = find_safe_future(len);
    x->safe = imm_range_intersect(past, future);

    struct imm_range tmp = {0};

    imm_range_subtract(future, x->safe, &x->safe_future, &tmp);
    if (imm_range_empty(x->safe_future)) imm_range_swap(&x->safe_future, &tmp);
    assert(imm_range_empty(tmp));

    imm_range_subtract(past, x->safe, &tmp, &x->safe_past);
    if (imm_range_empty(x->safe_past)) imm_range_swap(&x->safe_past, &tmp);
    assert(imm_range_empty(tmp));

    if (imm_range_empty(x->safe))
        imm_range_set(&x->unsafe, x->safe_future.b, x->safe_past.a);
    else
        imm_range_set(&x->unsafe, x->safe.b, x->safe.b);

    assert(imm_range_empty(x->safe) || imm_range_empty(x->unsafe));
}
