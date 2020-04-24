#ifndef IO_H
#define IO_H

#include "imm/io.h"
#include <inttypes.h>

struct dp_emission;
struct dp_state_table;
struct imm_abc;
struct imm_hmm;
struct mstate;
struct seq_code;

struct imm_io
{
    struct imm_abc const*        abc;
    struct imm_hmm*              hmm;
    struct mstate**              mstates;
    struct imm_state const**     states;
    uint32_t                     nstates;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table const* trans_table;
    struct dp_state_table const* state_table;
    struct imm_dp const*         dp;
    struct imm_io_vtable         vtable;
};

int io_read_abc(struct imm_io* io, FILE* stream);

#endif
