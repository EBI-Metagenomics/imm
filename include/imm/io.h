#ifndef IMM_IO_H
#define IMM_IO_H

#include "imm/export.h"
#include <stdio.h>

struct imm_hmm;
struct imm_dp;
struct imm_io;

IMM_EXPORT int imm_io_write(FILE* stream, struct imm_hmm const* hmm, struct imm_dp const* dp);
IMM_EXPORT struct imm_io const* imm_io_read(FILE* stream);

#endif
