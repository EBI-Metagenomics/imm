#ifndef IO_H
#define IO_H

#include "imm/float.h"
#include <stdint.h>
#include <stdio.h>

void write_array_u32_flt(uint32_t *size, imm_float const *data, FILE *stream);
void write_array_u32_u32(uint32_t *size, unsigned const *data, FILE *stream);

imm_float *read_array_u32_flt(imm_float *data, FILE *stream);
unsigned *read_array_u32_u32(unsigned *data, FILE *stream);
uint16_t *read_array_u16_flt_size(uint16_t *data, unsigned size, FILE *stream);

#endif
