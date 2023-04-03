#include "expect.h"
#include "array_size.h"
#include "lip/1darray/1darray.h"
#include "lip/lip.h"
#include <string.h>

bool imm_expect_map_size(struct lip_file *x, unsigned size)
{
  unsigned sz = 0;
  lip_read_map_size(x, &sz);
  return size == sz;
}

bool imm_expect_map_key(struct lip_file *x, char const *key)
{
  unsigned size = 0;
  char buf[16] = {0};

  lip_read_str_size(x, &size);
  if (size > imm_array_size(buf)) x->error = true;

  lip_read_str_data(x, size, buf);
  if (size != (unsigned)strlen(key)) x->error = true;
  return strncmp(key, buf, size) == 0;
}

bool imm_expect_1darr_u8(struct lip_file *x, unsigned size, uint8_t *arr)
{
  unsigned sz = 0;
  enum lip_1darray_type type = 0;
  lip_read_1darray_size_type(x, &sz, &type);
  if (size != sz) goto error;
  lip_read_1darray_int_data(x, size, arr);
  return !x->error;

error:
  x->error = true;
  return false;
}

bool imm_expect_1darr_u8_type(struct lip_file *x, unsigned size, uint8_t *arr)
{
  unsigned sz = 0;
  enum lip_1darray_type type = 0;
  lip_read_1darray_size_type(x, &sz, &type);
  if (size != sz) goto error;
  if (type != LIP_1DARRAY_UINT8) goto error;
  lip_read_1darray_int_data(x, size, arr);
  return !x->error;

error:
  x->error = true;
  return false;
}

bool imm_expect_1darr_float(struct lip_file *x, unsigned size, float *arr)
{
  unsigned sz = 0;
  enum lip_1darray_type type = 0;
  lip_read_1darray_size_type(x, &sz, &type);
  if (size != sz) goto error;
  lip_read_1darray_float_data(x, size, arr);
  return !x->error;

error:
  x->error = true;
  return false;
}

bool imm_expect_1darr_float_type(struct lip_file *x, unsigned size, float *arr)
{
  unsigned sz = 0;
  enum lip_1darray_type type = 0;
  lip_read_1darray_size_type(x, &sz, &type);
  if (size != sz) goto error;
  if (type != LIP_1DARRAY_F32) goto error;
  lip_read_1darray_float_data(x, size, arr);
  return !x->error;

error:
  x->error = true;
  return false;
}
