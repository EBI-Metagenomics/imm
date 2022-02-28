#include "expect.h"
#include "imm/support.h"
#include "lite_pack.h"

bool expect_1darray_u8(struct lip_file *file, unsigned size, uint8_t arr[])
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

bool expect_1darray_u8_type(struct lip_file *file, unsigned size, uint8_t arr[])
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

bool expect_1darray_float(struct lip_file *file, unsigned size, imm_float arr[])
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

bool expect_1darray_float_type(struct lip_file *file, unsigned size,
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
