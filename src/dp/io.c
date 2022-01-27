#include "io.h"
#include "code.h"
#include "dp/emis.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "xcmp.h"
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

enum imm_rc imm_dp_write(struct imm_dp const *dp, FILE *file)
{
    enum imm_rc rc = IMM_SUCCESS;
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    cmp_ctx_t ctx = {0};
    cmp_setup(&ctx, file);

    ERET(!cmp_write_map(&ctx, 12), IMM_IOERROR);

    /* emission */
    ERET(!XCMP_WRITE_STR(&ctx, KEY_EMIS_SCORE), IMM_IOERROR);
    size = emis_score_size(&dp->emis, nstates);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!io_write_imm_float(&ctx, dp->emis.score[i]), IMM_IOERROR);

    ERET(!XCMP_WRITE_STR(&ctx, KEY_EMIS_OFFSET), IMM_IOERROR);
    size = emis_offset_size(nstates);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_uinteger(&ctx, dp->emis.offset[i]);

    /* trans_table */
    ERET(!XCMP_WRITE_STR(&ctx, KEY_TRANS_SIZE), IMM_IOERROR);
    ERET(!cmp_write_uinteger(&ctx, dp->trans_table.ntrans), IMM_IOERROR);

    size = dp->trans_table.ntrans;
    ERET(!XCMP_WRITE_STR(&ctx, KEY_TRANS_SCORE), IMM_IOERROR);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!io_write_imm_float(&ctx, dp->trans_table.trans[i].score),
             IMM_IOERROR);

    size = dp->trans_table.ntrans;
    ERET(!XCMP_WRITE_STR(&ctx, KEY_TRANS_SRC), IMM_IOERROR);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!cmp_write_uinteger(&ctx, dp->trans_table.trans[i].src),
             IMM_IOERROR);

    size = trans_table_offsize(nstates);
    ERET(!XCMP_WRITE_STR(&ctx, KEY_TRANS_OFFSET), IMM_IOERROR);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!cmp_write_uinteger(&ctx, dp->trans_table.offset[i]), IMM_IOERROR);

    /* state_table */
    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_SIZE), IMM_IOERROR);
    ERET(!cmp_write_uinteger(&ctx, dp->state_table.nstates), IMM_IOERROR);

    size = dp->state_table.nstates;
    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_IDS), IMM_IOERROR);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!cmp_write_uinteger(&ctx, dp->state_table.ids[i]), IMM_IOERROR);

    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_START), IMM_IOERROR);
    ERET(!cmp_write_uinteger(&ctx, dp->state_table.start.state), IMM_IOERROR);
    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_LPROB), IMM_IOERROR);
    ERET(!io_write_imm_float(&ctx, dp->state_table.start.lprob), IMM_IOERROR);
    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_END), IMM_IOERROR);
    ERET(!cmp_write_uinteger(&ctx, dp->state_table.end_state_idx), IMM_IOERROR);

    ERET(!XCMP_WRITE_STR(&ctx, KEY_STATE_SPAN), IMM_IOERROR);
    ERET(!cmp_write_array(&ctx, size), IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!cmp_write_uinteger(&ctx, span_zip(dp->state_table.span + i)),
             IMM_IOERROR);

cleanup:
    return rc;
}

enum imm_rc imm_dp_read(struct imm_dp *dp, FILE *file)
{
    enum imm_rc rc = IMM_SUCCESS;
    uint64_t u64 = 0;
    uint32_t size = 0;
    cmp_ctx_t ctx = {0};
    struct imm_dp_emis *e = NULL;
    struct imm_dp_trans_table *tt = NULL;
    struct imm_dp_state_table *st = NULL;

    cmp_setup(&ctx, file);

    ERET(!xcmp_expect_map(&ctx, 12), IMM_IOERROR);

    /* emission */
    ERET(!XCMP_READ_KEY(&ctx, KEY_EMIS_SCORE), IMM_IOERROR);
    e = &dp->emis;
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->score = realloc(e->score, sizeof(*e->score) * size);
    ERET(!e->score && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!io_read_imm_float(&ctx, e->score + i), IMM_IOERROR);

    ERET(!XCMP_READ_KEY(&ctx, KEY_EMIS_OFFSET), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->offset = realloc(e->offset, sizeof(*e->offset) * size);
    ERET(!e->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
        e->offset[i] = (unsigned)u64;
    }

    /* trans_table */
    ERET(!XCMP_READ_KEY(&ctx, KEY_TRANS_SIZE), IMM_IOERROR);
    tt = &dp->trans_table;
    ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
    tt->ntrans = (unsigned)u64;

    ERET(!XCMP_READ_KEY(&ctx, KEY_TRANS_SCORE), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERET(tt->ntrans != size, IMM_PARSEERROR);
    tt->trans = realloc(tt->trans, sizeof(*tt->trans) * size);
    ERET(!tt->trans && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERET(!io_read_imm_float(&ctx, &tt->trans[i].score), IMM_IOERROR);

    ERET(!XCMP_READ_KEY(&ctx, KEY_TRANS_SRC), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERET(tt->ntrans != size, IMM_PARSEERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
        tt->trans[i].src = (imm_state_idx_t)u64;
    }

    ERET(!XCMP_READ_KEY(&ctx, KEY_TRANS_OFFSET), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    tt->offset = realloc(tt->offset, sizeof(*tt->offset) * size);
    ERET(!tt->offset && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
        tt->offset[i] = (imm_trans_idx_t)u64;
    }

    /* state_table */
    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_SIZE), IMM_IOERROR);
    st = &dp->state_table;
    ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
    st->nstates = (unsigned)u64;

    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_IDS), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERET(st->nstates != size, IMM_PARSEERROR);
    st->ids = realloc(st->ids, sizeof(*st->ids) * size);
    ERET(!st->ids && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
        st->ids[i] = (imm_state_id_t)u64;
    }

    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_START), IMM_IOERROR);
    ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
    st->start.state = (imm_state_idx_t)u64;
    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_LPROB), IMM_IOERROR);
    ERET(!io_read_imm_float(&ctx, &st->start.lprob), IMM_IOERROR);
    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_END), IMM_IOERROR);
    ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
    st->end_state_idx = (unsigned)u64;

    ERET(!XCMP_READ_KEY(&ctx, KEY_STATE_SPAN), IMM_IOERROR);
    ERET(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERET(st->nstates != size, IMM_PARSEERROR);
    st->span = realloc(st->span, sizeof(*st->span) * size);
    ERET(!st->span && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERET(!cmp_read_uinteger(&ctx, &u64), IMM_IOERROR);
        ERET(!span_unzip(st->span + i, (uint16_t)u64), IMM_PARSEERROR);
        ERET(st->span[i].max > IMM_STATE_MAX_SEQLEN, IMM_PARSEERROR);
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
