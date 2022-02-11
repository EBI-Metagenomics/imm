#include "code.h"
#include "dp/emis.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "lite_pack/1darray.h"
#include "lite_pack/ctx/file.h"
#include "lite_pack/lite_pack.h"
#include "xlip.h"
#include <assert.h>
#include <stdlib.h>

static_assert(sizeof(imm_nstates_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_seqlen_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_trans_idx_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_id_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_idx_t) == sizeof(uint16_t), "wrong types");

#define KEY_EMIS_SCORE "emis_score"
#define KEY_EMIS_OFFSET "emis_offset"
#define KEY_TRANS_SIZE "trans_size"
#define KEY_TRANS_SCORE "trans_score"
#define KEY_TRANS_SRC "trans_src"
#define KEY_TRANS_OFFSET "trans_offset"
#define KEY_STATE_SIZE "state_size"
#define KEY_STATE_IDS "state_ids"
#define KEY_STATE_START "state_start"
#define KEY_STATE_LPROB "state_lprob"
#define KEY_STATE_END "state_end"
#define KEY_STATE_SPAN "state_span"

#define ERET(expr, e)                                                          \
    do                                                                         \
    {                                                                          \
        if (!!(expr))                                                          \
        {                                                                      \
            rc = e;                                                            \
            goto cleanup;                                                      \
        }                                                                      \
    } while (0)

#ifdef IMM_DOUBLE_PRECISION
#define REAL_BYTES 8
#define USER_EXT CWP_ITEM_USER_EXT_1
#else
#define REAL_BYTES 4
#define USER_EXT CWP_ITEM_USER_EXT_0
#endif

enum imm_rc imm_dp_pack(struct imm_dp const *dp, struct lip_ctx_file *ctx)
{
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    lip_write_map_size(ctx, 10);

    /* emission */
    xlip_write_cstr(ctx, KEY_EMIS_SCORE);
    size = emis_score_size(&dp->emis, nstates);
    lip_write_1darray_size_type(ctx, size, XLIP_1DARRAY_FLOAT);
    lip_write_1darray_float(ctx, size, dp->emis.score);

    xlip_write_cstr(ctx, KEY_EMIS_OFFSET);
    size = emis_offset_size(nstates);
    lip_write_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        lip_read_int(ctx, dp->emis.offset + i);

    /* trans_table */
    // xlip_write_cstr(ctx, KEY_TRANS_SIZE);
    // cw_pack_unsigned(ctx, dp->trans_table.ntrans);

    size = dp->trans_table.ntrans;
    xlip_write_cstr(ctx, KEY_TRANS_SCORE);
    lip_write_1darray_size_type(ctx, size, XLIP_1DARRAY_FLOAT);
    for (unsigned i = 0; i < size; ++i)
        lip_write_1darray_float(ctx, 1, &dp->trans_table.trans[i].score);

    xlip_write_cstr(ctx, KEY_TRANS_SRC);
    lip_write_1darray_size_type(ctx, size, LIP_1DARRAY_UINT16);
    for (unsigned i = 0; i < size; ++i)
        lip_write_1darray_int_data(ctx, 1, &dp->trans_table.trans[i].src);

    size = trans_table_offsize(nstates);
    xlip_write_cstr(ctx, KEY_TRANS_OFFSET);
    lip_write_1darray_size_type(ctx, size, LIP_1DARRAY_UINT16);
    lip_write_1darray_int_data(ctx, size, dp->trans_table.offset);

    /* state_table */
    // xlip_write_cstr(ctx, KEY_STATE_SIZE);
    // cw_pack_unsigned(ctx, dp->state_table.nstates);

    size = dp->state_table.nstates;
    xlip_write_cstr(ctx, KEY_STATE_IDS);
    lip_write_1darray_size_type(ctx, size, LIP_1DARRAY_UINT16);
    lip_write_1darray_int_data(ctx, size, dp->state_table.ids);

    xlip_write_cstr(ctx, KEY_STATE_START);
    lip_write_int(ctx, dp->state_table.start.state);
    xlip_write_cstr(ctx, KEY_STATE_LPROB);
    lip_write_float(ctx, dp->state_table.start.lprob);
    xlip_write_cstr(ctx, KEY_STATE_END);
    lip_write_int(ctx, dp->state_table.end_state_idx);

    xlip_write_cstr(ctx, KEY_STATE_SPAN);
    lip_write_1darray_size_type(ctx, size, LIP_1DARRAY_UINT16);
    for (unsigned i = 0; i < size; ++i)
    {
        uint16_t u = span_zip(dp->state_table.span + i);
        lip_write_1darray_int_data(ctx, 1, &u);
    }

    return ctx->error ? IMM_IOERROR : IMM_SUCCESS;
}

enum imm_rc imm_dp_unpack(struct imm_dp *dp, struct lip_ctx_file *ctx)
{
    enum imm_rc rc = IMM_SUCCESS;
    unsigned size = 0;
    struct imm_dp_emis *e = NULL;
    struct imm_dp_trans_table *tt = NULL;
    struct imm_dp_state_table *st = NULL;

    if (cw_unpack_next_map_size(ctx) != 12) return IMM_FAILURE;

    /* emission */
    if (!cw_unpack_next_cstr_expect(ctx, KEY_EMIS_SCORE)) return IMM_FAILURE;
    e = &dp->emis;
    cw_unpack_next(ctx);
    // size = cw_unpack_next_array_size(ctx);
    size = ctx->item.as.ext.length / REAL_BYTES;
    // e->score = realloc(e->score, sizeof(*e->score) * size);
    e->score = realloc(e->score, ctx->item.as.ext.length);
    ERET(!e->score && size > 0, IMM_IOERROR);
    memcpy(e->score, ctx->item.as.ext.start, ctx->item.as.ext.length);
    // for (unsigned i = 0; i < size; ++i)
    //     e->score[i] = cw_unpack_next_imm_float(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_EMIS_OFFSET)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    e->offset = realloc(e->offset, sizeof(*e->offset) * size);
    ERET(!e->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        e->offset[i] = cw_unpack_next_unsigned(ctx);

    /* trans_table */
    if (!cw_unpack_next_cstr_expect(ctx, KEY_TRANS_SIZE)) return IMM_FAILURE;
    tt = &dp->trans_table;
    tt->ntrans = cw_unpack_next_unsigned(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_TRANS_SCORE)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    ERET(tt->ntrans != size, IMM_PARSEERROR);
    tt->trans = realloc(tt->trans, sizeof(*tt->trans) * size);
    ERET(!tt->trans && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        tt->trans[i].score = cw_unpack_next_imm_float(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_TRANS_SRC)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    ERET(tt->ntrans != size, IMM_PARSEERROR);
    for (unsigned i = 0; i < size; ++i)
        tt->trans[i].src = cw_unpack_next_unsigned16(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_TRANS_OFFSET)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    tt->offset = realloc(tt->offset, sizeof(*tt->offset) * size);
    ERET(!tt->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        tt->offset[i] = cw_unpack_next_unsigned16(ctx);

    /* state_table */
    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_SIZE)) return IMM_FAILURE;
    st = &dp->state_table;
    st->nstates = cw_unpack_next_unsigned(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_IDS)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    ERET(st->nstates != size, IMM_PARSEERROR);
    st->ids = realloc(st->ids, sizeof(*st->ids) * size);
    ERET(!st->ids && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        st->ids[i] = cw_unpack_next_unsigned16(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_START)) return IMM_FAILURE;
    st->start.state = cw_unpack_next_unsigned16(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_LPROB)) return IMM_FAILURE;
    st->start.lprob = cw_unpack_next_imm_float(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_END)) return IMM_FAILURE;
    st->end_state_idx = cw_unpack_next_unsigned(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, KEY_STATE_SPAN)) return IMM_FAILURE;
    size = cw_unpack_next_array_size(ctx);
    ERET(st->nstates != size, IMM_PARSEERROR);
    st->span = realloc(st->span, sizeof(*st->span) * size);
    ERET(!st->span && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!span_unzip(st->span + i, cw_unpack_next_unsigned16(ctx)),
             IMM_PARSEERROR);
        ERET(st->span[i].max > IMM_STATE_MAX_SEQLEN, IMM_PARSEERROR);
    }

    if (!ctx->return_code) return IMM_SUCCESS;

cleanup:
    if (e)
    {
        free(e->score);
        e->score = NULL;
        free(e->offset);
        e->offset = NULL;
    }
    if (tt)
    {
        free(tt->trans);
        tt->trans = NULL;
        free(tt->offset);
        tt->offset = NULL;
    }
    if (st)
    {
        free(st->ids);
        st->ids = NULL;
        free(st->span);
        st->span = NULL;
    }
    return rc;
}
