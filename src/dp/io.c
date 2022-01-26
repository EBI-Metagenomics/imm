#include "io.h"
#include "cmp/cmp.h"
#include "code.h"
#include "dp/emis.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include <assert.h>
#include <stdlib.h>

static_assert(sizeof(imm_nstates_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_seqlen_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_trans_idx_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_id_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_idx_t) == sizeof(uint16_t), "wrong types");

enum imm_rc imm_dp_write(struct imm_dp const *dp, FILE *file)
{
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    cmp_ctx_t ctx = {0};
    cmp_setup(&ctx, file);
    cmp_write_map(&ctx, 12);

    /* emission */
    CMP_WRITE_STR(&ctx, "emission_score");
    size = emis_score_size(&dp->emis, nstates);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        io_write_imm_float(&ctx, dp->emis.score[i]);

    CMP_WRITE_STR(&ctx, "emission_offset");
    size = emis_offset_size(nstates);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u32(&ctx, (uint32_t)dp->emis.offset[i]);

    /* trans_table */
    CMP_WRITE_STR(&ctx, "trans_table_ntrans");
    cmp_write_u16(&ctx, (uint16_t)dp->trans_table.ntrans);

    size = dp->trans_table.ntrans;
    CMP_WRITE_STR(&ctx, "trans_table_score");
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        io_write_imm_float(&ctx, dp->trans_table.trans[i].score);

    size = dp->trans_table.ntrans;
    CMP_WRITE_STR(&ctx, "trans_table_src");
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->trans_table.trans[i].src);

    size = trans_table_offsize(nstates);
    CMP_WRITE_STR(&ctx, "trans_table_offset");
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->trans_table.offset[i]);

    /* state_table */
    CMP_WRITE_STR(&ctx, "state_table_nstates");
    cmp_write_u16(&ctx, (uint16_t)dp->state_table.nstates);

    size = dp->state_table.nstates;
    CMP_WRITE_STR(&ctx, "state_table_ids");
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->state_table.ids[i]);

    CMP_WRITE_STR(&ctx, "state_table_start_state");
    cmp_write_u16(&ctx, dp->state_table.start.state);
    CMP_WRITE_STR(&ctx, "state_table_start_lprob");
    io_write_imm_float(&ctx, dp->state_table.start.lprob);
    CMP_WRITE_STR(&ctx, "state_table_end_state");
    cmp_write_u16(&ctx, (uint16_t)dp->state_table.end_state_idx);

    CMP_WRITE_STR(&ctx, "state_table_span");
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, span_zip(dp->state_table.span + i));

    return IMM_SUCCESS;
}

#define ERETURN(expr, e)                                                       \
    do                                                                         \
    {                                                                          \
        if (!!(expr))                                                          \
        {                                                                      \
            rc = e;                                                            \
            goto cleanup;                                                      \
        }                                                                      \
    } while (0)

enum imm_rc imm_dp_read(struct imm_dp *dp, FILE *file)
{
    enum imm_rc rc = IMM_SUCCESS;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint32_t size = 0;
    cmp_ctx_t ctx = {0};
    cmp_setup(&ctx, file);
    cmp_read_map(&ctx, &u32);
    assert(u32 == 12);

    struct imm_dp_emis *e = NULL;
    struct imm_dp_trans_table *tt = NULL;
    struct imm_dp_state_table *st = NULL;

    /* emission */
    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    e = &dp->emis;
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->score = realloc(e->score, sizeof(*e->score) * size);
    ERETURN(!e->score && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!io_read_imm_float(&ctx, e->score + i), IMM_IOERROR);

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->offset = realloc(e->offset, sizeof(*e->offset) * size);
    ERETURN(!e->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u32(&ctx, &u32), IMM_IOERROR);
        e->offset[i] = u32;
    }

    /* trans_table */
    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    tt = &dp->trans_table;
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    tt->ntrans = u16;

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(tt->ntrans != size, IMM_PARSEERROR);
    tt->trans = realloc(tt->trans, sizeof(*tt->trans) * size);
    ERETURN(!tt->trans && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!io_read_imm_float(&ctx, &tt->trans[i].score), IMM_IOERROR);

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(tt->ntrans != size, IMM_PARSEERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        tt->trans[i].src = u16;
    }

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    tt->offset = realloc(tt->offset, sizeof(*tt->offset) * size);
    ERETURN(!tt->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        tt->offset[i] = u16;
    }

    /* state_table */
    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    st = &dp->state_table;
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    st->nstates = u16;

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(st->nstates != size, IMM_PARSEERROR);
    st->ids = realloc(st->ids, sizeof(*st->ids) * size);
    ERETURN(!st->ids && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!cmp_read_u16(&ctx, st->ids + i), IMM_IOERROR);

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_u16(&ctx, &st->start.state), IMM_IOERROR);
    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!io_read_imm_float(&ctx, &st->start.lprob), IMM_IOERROR);
    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    st->end_state_idx = u16;

    ERETURN(!cmp_skip_str(&ctx), IMM_IOERROR);
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(st->nstates != size, IMM_PARSEERROR);
    st->span = realloc(st->span, sizeof(*st->span) * size);
    ERETURN(!st->span && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        ERETURN(!span_unzip(st->span + i, u16), IMM_PARSEERROR);
        ERETURN(st->span[i].max > IMM_STATE_MAX_SEQLEN, IMM_PARSEERROR);
    }

    return rc;

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
