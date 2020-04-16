#ifndef IO_H
#define IO_H

#include <inttypes.h>

struct imm_abc;
struct imm_hmm;
struct mstate;

struct imm_io
{
    struct imm_abc const* abc;
    struct imm_hmm*       hmm;
    struct mstate**       mstates;
    uint32_t              nstates;
};

static inline struct mstate* io_mstate(struct imm_io const* io, uint32_t i)
{
    return io->mstates[i];
}

#endif
