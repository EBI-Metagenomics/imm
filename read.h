#ifndef READ_H
#define READ_H

#include "lio.h"
#include <stdint.h>

static inline int read_map(struct lio_reader *x, uint32_t *size)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_map(buf, size))) return 1;
  return 0;
}

static inline int read_array(struct lio_reader *x, uint32_t *size)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_array(buf, size))) return 1;
  return 0;
}

static inline int read_string(struct lio_reader *x, uint32_t *size)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_string(buf, size))) return 1;
  return 0;
}

static inline int read_cstring(struct lio_reader *x, uint32_t size, char *string)
{
  uint32_t length = 0;
  if (read_string(x, &length)) return 1;
  if (length >= size) return 1;
  if (lio_readb(x, length, (unsigned char *)string)) return 1;
  string[length] = '\0';
  return 0;
}

static inline int read_float(struct lio_reader *x, float *data)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_float(buf, data))) return 1;
  return 0;
}

static inline int read_i8(struct lio_reader *x, int8_t *data)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_int(buf, data))) return 1;
  return 0;
}

static inline int read_i16(struct lio_reader *x, int16_t *data)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_int(buf, data))) return 1;
  return 0;
}

static inline int read_i32(struct lio_reader *x, int32_t *data)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_int(buf, data))) return 1;
  return 0;
}

static inline int read_u16(struct lio_reader *x, uint16_t *data)
{
  unsigned char *buf = NULL;
  if (lio_read(x, &buf)) return 1;
  if (lio_free(x, lip_unpack_int(buf, data))) return 1;
  return 0;
}

#define read_int(buffer, data)                                                 \
  _Generic((data),                                                             \
      int8_t *: read_i8,                                                       \
      int16_t *: read_i16,                                                     \
      int32_t *: read_i32,                                                     \
      uint16_t *: read_u16)(buffer, data)

#endif
