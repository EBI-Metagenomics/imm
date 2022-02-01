#ifndef XCW_H
#define XCW_H

#include "cwpack.h"
#include "cwpack_utils.h"

#define cw_pack_imm_float(ctx, v)                                              \
    _Generic((v), float : cw_pack_float, double : cw_pack_double)(ctx, v)

#ifdef IMM_DOUBLE_PRECISION
#define cw_unpack_next_imm_float(ctx) cw_unpack_next_double(ctx)
#else
#define cw_unpack_next_imm_float(ctx) cw_unpack_next_float(ctx)
#endif

#endif
