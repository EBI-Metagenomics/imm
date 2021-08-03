#include "imm/rnd.h"
#include "third-party/xrandom.h"

static inline struct xrandom to_xrandom(struct imm_rnd r)
{
    return (struct xrandom){r.data[0], r.data[1], r.data[2], r.data[3]};
}

static inline struct imm_rnd from_xrandom(struct xrandom x)
{
    return (struct imm_rnd){x.data[0], x.data[1], x.data[2], x.data[3]};
}

struct imm_rnd imm_rnd(uint64_t seed) { return from_xrandom(xrandom(seed)); }

double imm_rnd_dbl(struct imm_rnd *rnd)
{
    struct xrandom x = to_xrandom(*rnd);
    double val = xrandom_dbl(&x);
    *rnd = from_xrandom(x);
    return val;
}

uint64_t imm_rnd_u64(struct imm_rnd *rnd)
{
    struct xrandom x = to_xrandom(*rnd);
    uint64_t val = xrandom_u64(&x);
    *rnd = from_xrandom(x);
    return val;
}
