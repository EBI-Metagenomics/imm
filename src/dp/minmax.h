#ifndef DP_MINMAX_H
#define DP_MINMAX_H

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

static inline unsigned min_u(unsigned x, unsigned y) { return x < y ? x : y; }
static inline unsigned max_u(unsigned x, unsigned y) { return x > y ? x : y; }

#endif
