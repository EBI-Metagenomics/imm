#include "xlip.h"
#include "imm/support.h"
#include "lite_pack/ctx/file.h"
#include "lite_pack/lite_pack.h"

bool xlip_expect_map(struct lip_ctx_file *ctx, unsigned size)
{
    unsigned sz = 0;
    lip_read_map_size(ctx, &size);
    return size == sz;
}

bool xlip_expect_key(struct lip_ctx_file *ctx, char const key[])
{
    unsigned size = 0;
    char buf[16] = {0};

    lip_read_str_size(ctx, &size);
    if (size > IMM_ARRAY_SIZE(buf)) return false;

    lip_read_str_data(ctx, size, buf);
    if (size != (unsigned)strlen(key)) return false;
    return strncmp(key, buf, size) == 0;
}

void xlip_write_cstr(struct lip_ctx_file *ctx, char const val[])
{
    unsigned size = (unsigned)strlen(val);
    lip_write_str_size(ctx, size);
    lip_write_str_data(ctx, size, val);
}

void xlip_read_cstr(struct lip_ctx_file *ctx, unsigned size, char val[])
{
    val[0] = 0;
    unsigned sz = 0;
    lip_read_str_size(ctx, &sz);
    if (sz > size)
    {
        ctx->error = true;
        return;
    }
    lip_read_str_data(ctx, sz, val);
    val[sz] = 0;
}
