#ifndef IMM_IO_H
#define IMM_IO_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_io;
struct imm_state;

IMM_EXPORT int imm_io_write(FILE* stream, struct imm_hmm const* hmm, struct imm_dp const* dp);
IMM_EXPORT struct imm_io const*    imm_io_read(FILE* stream);
IMM_EXPORT void                    imm_io_destroy(struct imm_io const* io);
IMM_EXPORT struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i);
IMM_EXPORT uint32_t                imm_io_nstates(struct imm_io const* io);
IMM_EXPORT struct imm_abc const*   imm_io_abc(struct imm_io const* io);
IMM_EXPORT struct imm_hmm*         imm_io_hmm(struct imm_io const* io);
IMM_EXPORT struct imm_dp const*    imm_io_dp(struct imm_io const* io);

#endif
