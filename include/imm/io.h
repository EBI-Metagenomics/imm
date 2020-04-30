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

struct imm_io_vtable
{
    void (*destroy)(struct imm_io const* io);
    struct imm_state const* (*read_state)(struct imm_io const* io, FILE* stream, uint8_t type_id);
    int (*write)(struct imm_io const* io, FILE* stream);
    void (*destroy_on_read_failure)(struct imm_io const* io);
};

IMM_EXPORT struct imm_io const*    imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp);
IMM_EXPORT struct imm_io const*    imm_io_create_from_file(FILE* stream);
IMM_EXPORT void                    imm_io_destroy(struct imm_io const* io);
IMM_EXPORT int                     imm_io_write(struct imm_io const* io, FILE* stream);
IMM_EXPORT struct imm_state const* imm_io_state(struct imm_io const* io, uint32_t i);
IMM_EXPORT uint32_t                imm_io_nstates(struct imm_io const* io);
IMM_EXPORT struct imm_abc const*   imm_io_abc(struct imm_io const* io);
IMM_EXPORT struct imm_hmm*         imm_io_hmm(struct imm_io const* io);
IMM_EXPORT struct imm_dp const*    imm_io_dp(struct imm_io const* io);

IMM_EXPORT struct imm_io*          __imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp,
                                                   void* derived);
IMM_EXPORT void*                   __imm_io_derived(struct imm_io const* io);
IMM_EXPORT void                    __imm_io_destroy(struct imm_io const* io);
IMM_EXPORT void                    __imm_io_destroy_on_read_failure(struct imm_io const* io);
IMM_EXPORT struct imm_io*          __imm_io_new(void* derived);
IMM_EXPORT int                     __imm_io_read(struct imm_io* io, FILE* stream);
IMM_EXPORT int                     __imm_io_read_abc(struct imm_io* io, FILE* stream);
IMM_EXPORT int                     __imm_io_read_dp(struct imm_io* io, FILE* stream);
IMM_EXPORT int                     __imm_io_read_hmm(struct imm_io* io, FILE* stream);
IMM_EXPORT struct imm_state const* __imm_io_read_state(struct imm_io const* io, FILE* stream,
                                                       uint8_t type_id);
IMM_EXPORT void                    __imm_io_set_abc(struct imm_io* io, struct imm_abc const* abc);
IMM_EXPORT struct imm_io_vtable*   __imm_io_vtable(struct imm_io* io);
IMM_EXPORT int                     __imm_io_write_abc(struct imm_io const* io, FILE* stream);
IMM_EXPORT int                     __imm_io_write_dp(struct imm_io const* io, FILE* stream);
IMM_EXPORT int                     __imm_io_write_hmm(struct imm_io const* io, FILE* stream);

#endif
