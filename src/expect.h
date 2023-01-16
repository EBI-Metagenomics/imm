#ifndef EXPECT_H
#define EXPECT_H

#include "imm/float.h"
#include "imm/support.h"
#include "lip.h"
#include <stdbool.h>
#include <stdint.h>

bool imm_expect_map_size(struct lip_file *file, unsigned size);

bool imm_expect_map_key(struct lip_file *file, char const key[]);

bool imm_expect_1darr_u8(struct lip_file *, unsigned size, uint8_t arr[]);
bool imm_expect_1darr_u8_type(struct lip_file *file, unsigned size,
                              uint8_t arr[]);
bool imm_expect_1darr_float(struct lip_file *, unsigned size, imm_float arr[]);
bool imm_expect_1darr_float_type(struct lip_file *file, unsigned size,
                                 imm_float arr[]);

#endif
