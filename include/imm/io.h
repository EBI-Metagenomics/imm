#ifndef IMM_IO_H
#define IMM_IO_H

#include "imm/export.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct imm_output;
struct imm_input;
struct imm_model;
struct imm_state;

/* IMM_API int            imm_io_write(struct imm_io const* io, FILE* stream); */

/* IMM_API struct imm_io const*    imm_io_create(struct imm_hmm* hmm, struct imm_dp const* dp); */
IMM_API struct imm_io const* imm_io_create_from_file(FILE* stream);
/* IMM_API int                     imm_io_write(struct imm_io const* io, FILE* stream); */

#endif
