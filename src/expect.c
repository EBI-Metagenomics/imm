#include "expect.h"
#include "imm/support.h"
#include "lite_pack.h"
#include <string.h>

bool imm_expect_map_size(struct lip_file *file, unsigned size)
{
    unsigned sz = 0;
    lip_read_map_size(file, &sz);
    return size == sz;
}

bool imm_expect_map_key(struct lip_file *file, char const key[])
{
    unsigned size = 0;
    char buf[16] = {0};

    lip_read_str_size(file, &size);
    if (size > IMM_ARRAY_SIZE(buf)) file->error = true;

    lip_read_str_data(file, size, buf);
    if (size != (unsigned)strlen(key)) file->error = true;
    return strncmp(key, buf, size) == 0;
}

bool imm_expect_1darr_u8(struct lip_file *file, unsigned size, uint8_t arr[])
{
    unsigned sz = 0;
    enum lip_1darray_type type = 0;
    lip_read_1darray_size_type(file, &sz, &type);
    if (size != sz) goto error;
    lip_read_1darray_int_data(file, size, arr);
    return !file->error;

error:
    file->error = true;
    return false;
}

bool imm_expect_1darr_u8_type(struct lip_file *file, unsigned size,
                              uint8_t arr[])
{
    unsigned sz = 0;
    enum lip_1darray_type type = 0;
    lip_read_1darray_size_type(file, &sz, &type);
    if (size != sz) goto error;
    if (type != LIP_1DARRAY_UINT8) goto error;
    lip_read_1darray_int_data(file, size, arr);
    return !file->error;

error:
    file->error = true;
    return false;
}

bool imm_expect_1darr_float(struct lip_file *file, unsigned size,
                            imm_float arr[])
{
    unsigned sz = 0;
    enum lip_1darray_type type = 0;
    lip_read_1darray_size_type(file, &sz, &type);
    if (size != sz) goto error;
    lip_read_1darray_float_data(file, size, arr);
    return !file->error;

error:
    file->error = true;
    return false;
}

bool imm_expect_1darr_float_type(struct lip_file *file, unsigned size,
                                 imm_float arr[])
{
    unsigned sz = 0;
    enum lip_1darray_type type = 0;
    lip_read_1darray_size_type(file, &sz, &type);
    if (size != sz) goto error;
    if (type != XLIP_1DARRAY_FLOAT) goto error;
    lip_read_1darray_float_data(file, size, arr);
    return !file->error;

error:
    file->error = true;
    return false;
}
