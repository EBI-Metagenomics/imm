#ifndef IO_H
#define IO_H

#include <inttypes.h>

struct dp_emission;
struct imm_abc;
struct imm_hmm;
struct mstate;
struct seq_code;

struct imm_io
{
    struct imm_abc const*  abc;
    struct imm_hmm*        hmm;
    struct mstate**        mstates;
    uint32_t               nstates;
    struct seq_code*       seq_code;
    struct dp_emission*    emission;
    struct dp_trans_table* trans_table;
};

static inline struct mstate* io_mstate(struct imm_io const* io, uint32_t i)
{
    return io->mstates[i];
}

#endif
