#include "code.h"
#include "dp/emis.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "xcw.h"
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

enum imm_rc imm_dp_pack(struct imm_dp const *dp, struct cw_pack_context *ctx)
{
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    cw_pack_map_size(ctx, 12);

    /* emission */
    cw_pack_cstr(ctx, KEY_EMIS_SCORE);
    size = emis_score_size(&dp->emis, nstates);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_imm_float(ctx, dp->emis.score[i]);

    cw_pack_cstr(ctx, KEY_EMIS_OFFSET);
    size = emis_offset_size(nstates);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_unsigned(ctx, dp->emis.offset[i]);

    /* trans_table */
    cw_pack_cstr(ctx, KEY_TRANS_SIZE);
    cw_pack_unsigned(ctx, dp->trans_table.ntrans);

    size = dp->trans_table.ntrans;
    cw_pack_cstr(ctx, KEY_TRANS_SCORE);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_imm_float(ctx, dp->trans_table.trans[i].score);

    size = dp->trans_table.ntrans;
    cw_pack_cstr(ctx, KEY_TRANS_SRC);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_unsigned(ctx, dp->trans_table.trans[i].src);

    size = trans_table_offsize(nstates);
    cw_pack_cstr(ctx, KEY_TRANS_OFFSET);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_unsigned(ctx, dp->trans_table.offset[i]);

    /* state_table */
    cw_pack_cstr(ctx, KEY_STATE_SIZE);
    cw_pack_unsigned(ctx, dp->state_table.nstates);

    size = dp->state_table.nstates;
    cw_pack_cstr(ctx, KEY_STATE_IDS);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_unsigned(ctx, dp->state_table.ids[i]);

    cw_pack_cstr(ctx, KEY_STATE_START);
    cw_pack_unsigned(ctx, dp->state_table.start.state);
    cw_pack_cstr(ctx, KEY_STATE_LPROB);
    cw_pack_imm_float(ctx, dp->state_table.start.lprob);
    cw_pack_cstr(ctx, KEY_STATE_END);
    cw_pack_unsigned(ctx, dp->state_table.end_state_idx);

    cw_pack_cstr(ctx, KEY_STATE_SPAN);
    cw_pack_array_size(ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cw_pack_unsigned(ctx, span_zip(dp->state_table.span + i));

    return ctx->return_code ? IMM_IOERROR : IMM_SUCCESS;
}

enum imm_rc imm_dp_unpack(struct imm_dp *dp, struct cw_unpack_context *ctx)
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
    size = cw_unpack_next_array_size(ctx);
    e->score = realloc(e->score, sizeof(*e->score) * size);
    ERET(!e->score && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        e->score[i] = cw_unpack_next_imm_float(ctx);

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
