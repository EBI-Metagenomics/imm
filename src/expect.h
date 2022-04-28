#ifndef EXPECT_H
#define EXPECT_H

#include "imm/float.h"
#include "imm/support.h"
#include "lite_pack.h"
#include <stdbool.h>
#include <stdint.h>

bool expect_map_size(struct lip_file *file, unsigned size);

bool expect_map_key(struct lip_file *file, char const key[]);

bool expect_1darray_u8(struct lip_file *, unsigned size, uint8_t arr[]);
bool expect_1darray_u8_type(struct lip_file *file, unsigned size,
                            uint8_t arr[]);
bool expect_1darray_float(struct lip_file *, unsigned size, imm_float arr[]);
bool expect_1darray_float_type(struct lip_file *file, unsigned size,
                               imm_float arr[]);

#endif
