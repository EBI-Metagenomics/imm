#ifndef IMM_DUMP_H
#define IMM_DUMP_H

#include "export.h"
#include <stddef.h>
#include <stdio.h>

IMM_API void imm_dump_array_f32(size_t, float const *, FILE *restrict);
IMM_API void imm_dump_array_f64(size_t, double const *, FILE *restrict);

#endif
