#include "xcmp.h"

bool xcmp_expect_str(struct cmp_ctx_s *ctx, char const *str, uint32_t size)
{
    uint32_t sz = 0;
    if (!cmp_read_str_size(ctx, &sz)) return false;
    if (sz != size) return false;

    int8_t c = 0;
    for (unsigned i = 0; i < sz; ++i)
    {
        if (!cmp_read_char(ctx, &c)) return false;
        if (c != str[i]) return false;
    }
    return str[sz] == 0;
}

bool xcmp_expect_map(struct cmp_ctx_s *ctx, uint32_t size)
{
    uint32_t u32 = 0;
    if (!cmp_read_map(ctx, &u32)) return false;
    return u32 == size;
}
