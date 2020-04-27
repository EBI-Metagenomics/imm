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

typedef struct imm_abc const* (*imm_io_read_abc_t)(FILE* stream, uint8_t type_id);
typedef void (*imm_io_destroy_t)(struct imm_io const* io);

struct imm_io_vtable
{
    imm_io_read_abc_t read_abc;
    imm_io_destroy_t  destroy;
};

IMM_EXPORT struct imm_io const*    imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_EXPORT struct imm_io const*    imm_io_create_from_file(FILE* stream);
IMM_EXPORT void                    imm_io_destroy_states(struct imm_io const* io);
IMM_EXPORT void                    imm_io_destroy(struct imm_io const* io);
IMM_EXPORT int                     imm_io_write(struct imm_io const* io, FILE* stream);
IMM_EXPORT struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i);
IMM_EXPORT uint32_t                imm_io_nstates(struct imm_io const* io);
IMM_EXPORT struct imm_abc const*   imm_io_abc(struct imm_io const* io);
IMM_EXPORT struct imm_hmm*         imm_io_hmm(struct imm_io const* io);
IMM_EXPORT struct imm_dp const*    imm_io_dp(struct imm_io const* io);

IMM_EXPORT void*                __imm_io_child(struct imm_io const* io);
IMM_EXPORT struct imm_io const* __imm_io_create_parent(struct imm_hmm* hmm, struct imm_dp const* dp,
                                                       struct imm_io_vtable vtable, void* child);
IMM_EXPORT void                 __imm_io_destroy_parent(struct imm_io const* io);

#endif
