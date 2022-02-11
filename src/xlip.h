#ifndef XLIP_H
#define XLIP_H

#include <stdbool.h>

#ifdef IMM_DOUBLE_PRECISION
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F64
#else
#define XLIP_1DARRAY_FLOAT LIP_1DARRAY_F32
#endif

struct lip_ctx_file;

bool xlip_expect_map(struct lip_ctx_file *ctx, unsigned size);
bool xlip_expect_key(struct lip_ctx_file *ctx, char const key[]);
void xlip_write_cstr(struct lip_ctx_file *ctx, char const val[]);
void xlip_read_cstr(struct lip_ctx_file *ctx, unsigned size, char val[]);

#endif
