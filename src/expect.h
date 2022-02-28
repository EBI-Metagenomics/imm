#ifndef EXPECT_H
#define EXPECT_H

#include "imm/float.h"
#include "imm/support.h"
#include "lite_pack.h"
#include <stdbool.h>
#include <stdint.h>

static inline bool expect_map_size(struct lip_file *file, unsigned size)
{
    unsigned sz = 0;
    lip_read_map_size(file, &sz);
    return size == sz;
}

static bool expect_map_key(struct lip_file *file, char const key[])
{
    unsigned size = 0;
    char buf[16] = {0};

    lip_read_str_size(file, &size);
    if (size > IMM_ARRAY_SIZE(buf)) file->error = true;

    lip_read_str_data(file, size, buf);
    if (size != (unsigned)strlen(key)) file->error = true;
    return strncmp(key, buf, size) == 0;
}

bool expect_1darray_u8(struct lip_file *, unsigned size, uint8_t arr[]);
bool expect_1darray_u8_type(struct lip_file *file, unsigned size,
                            uint8_t arr[]);
bool expect_1darray_float(struct lip_file *, unsigned size, imm_float arr[]);
bool expect_1darray_float_type(struct lip_file *file, unsigned size,
                               imm_float arr[]);

#endif
