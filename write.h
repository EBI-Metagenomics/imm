#ifndef WRITE_H
#define WRITE_H

#include <stdint.h>

struct lio_writer;

int write_cstring(struct lio_writer *, char const *string);
int write_float(struct lio_writer *, float data);
int write_int(struct lio_writer *, int data);
int write_array(struct lio_writer *, uint32_t size);
int write_map(struct lio_writer *x, uint32_t size);

#endif
