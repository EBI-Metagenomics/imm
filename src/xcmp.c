#include "xcmp.h"
#include <assert.h>

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

static_assert(sizeof(size_t) >= 8, "64-bits offset");
static_assert(sizeof(off_t) >= 8, "64-bits offset");

static bool read(struct cmp_ctx_s *ctx, void *data, size_t limit)
{
    FILE *fp = (FILE *)ctx->buf;
    return fread(data, sizeof(uint8_t), limit, fp) == (limit * sizeof(uint8_t));
}

static bool skip(struct cmp_ctx_s *ctx, size_t count)
{
    return fseeko((FILE *)ctx->buf, (off_t)count, SEEK_CUR) != -1;
}

static size_t write(struct cmp_ctx_s *ctx, const void *data, size_t count)
{
    return fwrite(data, sizeof(uint8_t), count, (FILE *)ctx->buf);
}

void xcmp_fsetup(struct cmp_ctx_s *ctx, FILE *fp)
{
    cmp_init(ctx, fp, read, skip, write);
}
