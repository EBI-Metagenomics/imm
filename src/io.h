#ifndef IO_H
#define IO_H

#include "cmp.h"
#include "imm/state_types.h"
#include "imm/trans.h"
#include "imm/types.h"
#include "support.h"
#include <assert.h>
#include <stdint.h>

static bool read_bytes(void *data, size_t sz, FILE *fh)
{
    return fread(data, sizeof(uint8_t), sz, fh) == (sz * sizeof(uint8_t));
}

static bool file_reader(cmp_ctx_t *ctx, void *data, size_t limit)
{
    return read_bytes(data, limit, (FILE *)ctx->buf);
}

static bool file_skipper(cmp_ctx_t *ctx, size_t count)
{
    IMM_BUG(count > ULONG_MAX);
    return fseek((FILE *)ctx->buf, (long)count, SEEK_CUR);
}

static size_t file_writer(cmp_ctx_t *ctx, const void *data, size_t count)
{
    return fwrite(data, sizeof(uint8_t), count, (FILE *)ctx->buf);
}

static inline bool cmp_write_imm_state_idx(cmp_ctx_t *ctx, unsigned s)
{
    static_assert(sizeof(imm_state_idx_t) == sizeof(uint16_t), "wrong types");
    return cmp_write_u16(ctx, (uint16_t)s);
}

static inline bool cmp_write_imm_state_seqlen(cmp_ctx_t *ctx, unsigned s)
{
    static_assert(sizeof(imm_state_seqlen_t) == sizeof(uint8_t), "wrong types");
    return cmp_write_u8(ctx, (uint8_t)s);
}

static inline bool cmp_write_imm_trans_idx(cmp_ctx_t *ctx, unsigned s)
{
    static_assert(sizeof(imm_trans_idx_t) == sizeof(uint16_t), "wrong types");
    return cmp_write_u16(ctx, (uint16_t)s);
}

static inline bool cmp_write_imm_nstates(cmp_ctx_t *ctx, unsigned s)
{
    static_assert(sizeof(imm_nstates_t) == sizeof(uint16_t), "wrong types");
    return cmp_write_u16(ctx, (uint16_t)s);
}

#endif
