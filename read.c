#include "read.h"
#include "lite_pack_io.h"

int read_map(struct lio_reader *x, uint32_t *size)
{
  if (lio_free(x, lip_unpack_map(lio_read(x), size))) return 1;
  return 0;
}

int read_array(struct lio_reader *x, uint32_t *size)
{
  if (lio_free(x, lip_unpack_array(lio_read(x), size))) return 1;
  return 0;
}

int read_string(struct lio_reader *x, uint32_t *size)
{
  if (lio_free(x, lip_unpack_string(lio_read(x), size))) return 1;
  return 0;
}

int read_cstring(struct lio_reader *x, uint32_t size, char *string)
{
  uint32_t length = 0;
  if (read_string(x, &length)) return 1;
  if (length >= size) return 1;
  if (lio_readb(x, length, (unsigned char *)string)) return 1;
  string[length] = '\0';
  return 0;
}

int read_float(struct lio_reader *x, float *data)
{
  if (lio_free(x, lip_unpack_float(lio_read(x), data))) return 1;
  return 0;
}

int read_i8(struct lio_reader *x, int8_t *data)
{
  if (lio_free(x, lip_unpack_int(lio_read(x), data))) return 1;
  return 0;
}

int read_i16(struct lio_reader *x, int16_t *data)
{
  if (lio_free(x, lip_unpack_int(lio_read(x), data))) return 1;
  return 0;
}

int read_i32(struct lio_reader *x, int32_t *data)
{
  if (lio_free(x, lip_unpack_int(lio_read(x), data))) return 1;
  return 0;
}

int read_u16(struct lio_reader *x, uint16_t *data)
{
  if (lio_free(x, lip_unpack_int(lio_read(x), data))) return 1;
  return 0;
}
