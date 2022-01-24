#ifndef IO_H
#define IO_H

#include "cmp/cmp.h"

#define io_write_imm_float(ctx, v)                                             \
    _Generic((v), float : cmp_write_float, double : cmp_write_double)(ctx, v)

#define io_read_imm_float(ctx, v)                                              \
    _Generic((v), float * : cmp_read_float, double * : cmp_read_double)(ctx, v)

#endif
