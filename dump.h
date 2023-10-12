#ifndef IMM_DUMP_H
#define IMM_DUMP_H

#include "compiler.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

IMM_API void imm_dump_array_f32(size_t, float const *, FILE *restrict);
IMM_API void imm_dump_array_f64(size_t, double const *, FILE *restrict);
IMM_API void imm_dump_array_u8(size_t, uint8_t const *, FILE *restrict);
IMM_API void imm_dump_array_u16(size_t, uint16_t const *, FILE *restrict);
IMM_API void imm_dump_array_u32(size_t, uint32_t const *, FILE *restrict);

#endif
