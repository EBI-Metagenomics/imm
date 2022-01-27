#ifndef XCMP_H
#define XCMP_H

#include "cmp/cmp.h"

#define __ARRSIZE(x) (sizeof(x) / sizeof(x[0]))
#define __STRSIZE(x) (__ARRSIZE(x) - 1)
#define __STRADDR(x) ((char const *)(char const(*)[__ARRSIZE(x)]){&(x)})

#define XCMP_WRITE_STR(ctx, msg)                                               \
    cmp_write_str(ctx, __STRADDR(msg), (uint32_t)(__STRSIZE(msg)))

bool xcmp_expect_str(struct cmp_ctx_s *ctx, char const *str, uint32_t size);

bool xcmp_expect_map(struct cmp_ctx_s *ctx, uint32_t size);

#define XCMP_READ_KEY(ctx, key)                                                \
    xcmp_expect_str(ctx, __STRADDR(key), (uint32_t)(__STRSIZE(key)))

#endif
