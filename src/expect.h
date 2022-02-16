#ifndef EXPECT_H
#define EXPECT_H

#include "imm/float.h"
#include <stdbool.h>
#include <stdint.h>

struct lip_io_file;

bool expect_map(struct lip_io_file *io, unsigned size);
bool expect_key(struct lip_io_file *io, char const key[]);
bool expect_1darray_u8(struct lip_io_file *, unsigned size, uint8_t arr[]);
bool expect_1darray_u8_type(struct lip_io_file *io, unsigned size, uint8_t type,
                            uint8_t arr[]);
bool expect_1darray_float(struct lip_io_file *, unsigned size, imm_float arr[]);
bool expect_1darray_float_type(struct lip_io_file *io, unsigned size,
                               uint8_t type, imm_float arr[]);

#endif
