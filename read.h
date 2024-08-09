#ifndef READ_H
#define READ_H

#include <stdint.h>

struct lio_reader;

int read_map(struct lio_reader *, uint32_t *size);
int read_array(struct lio_reader *, uint32_t *size);
int read_string(struct lio_reader *, uint32_t *size);
int read_cstring(struct lio_reader *, uint32_t size, char *string);
int read_float(struct lio_reader *, float *data);
int read_i8(struct lio_reader *, int8_t *data);
int read_i16(struct lio_reader *, int16_t *data);
int read_i32(struct lio_reader *, int32_t *data);
int read_u16(struct lio_reader *, uint16_t *data);

#define read_int(buffer, data)                                                 \
  _Generic((data),                                                             \
      int8_t *: read_i8,                                                       \
      int16_t *: read_i16,                                                     \
      int32_t *: read_i32,                                                     \
      uint16_t *: read_u16)(buffer, data)

#endif
