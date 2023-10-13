#ifndef IMM_EXPECT_H
#define IMM_EXPECT_H

#include <stdbool.h>
#include <stdint.h>

struct lip_file;

bool imm_expect_map_size(struct lip_file *, unsigned size);
bool imm_expect_map_key(struct lip_file *, char const *key);
bool imm_expect_1darr_i8(struct lip_file *, unsigned size, int8_t *arr);
bool imm_expect_1darr_i8_type(struct lip_file *, unsigned size, int8_t *arr);
bool imm_expect_1darr_float(struct lip_file *, unsigned size, float *arr);
bool imm_expect_1darr_float_type(struct lip_file *, unsigned size, float *arr);

#endif
