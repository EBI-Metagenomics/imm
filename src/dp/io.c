#include "code.h"
#include "dp/emis.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "error.h"
#include "expect.h"
#include "imm/dp.h"
#include "lite_pack.h"
#include "reallocf.h"
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
            rc = error(e);                                                     \
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

enum imm_rc imm_dp_pack(struct imm_dp const *dp, struct lip_file *f)
{
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    lip_write_map_size(f, 10);

    /* emission */
    lip_write_cstr(f, KEY_EMIS_SCORE);
    size = emis_score_size(&dp->emis, nstates);
    lip_write_1darray_float(f, size, dp->emis.score);

    lip_write_cstr(f, KEY_EMIS_OFFSET);
    lip_write_1darray_int(f, emis_offset_size(nstates), dp->emis.offset);

    /* trans_table */
    size = dp->trans_table.ntrans;
    lip_write_cstr(f, KEY_TRANS_SCORE);
    lip_write_1darray_size_type(f, size, XLIP_1DARRAY_FLOAT);
    for (unsigned i = 0; i < size; ++i)
        lip_write_1darray_float_item(f, dp->trans_table.trans[i].score);

    size = dp->trans_table.ntrans;
    lip_write_cstr(f, KEY_TRANS_SRC);
    lip_write_1darray_size_type(f, size, LIP_1DARRAY_UINT16);
    for (unsigned i = 0; i < size; ++i)
        lip_write_1darray_int_item(f, dp->trans_table.trans[i].src);

    size = trans_table_offsize(nstates);
    lip_write_cstr(f, KEY_TRANS_OFFSET);
    lip_write_1darray_int(f, size, dp->trans_table.offset);

    /* state_table */
    lip_write_cstr(f, KEY_STATE_IDS);
    lip_write_1darray_int(f, nstates, dp->state_table.ids);

    lip_write_cstr(f, KEY_STATE_START);
    lip_write_int(f, dp->state_table.start.state);
    lip_write_cstr(f, KEY_STATE_LPROB);
    lip_write_float(f, dp->state_table.start.lprob);
    lip_write_cstr(f, KEY_STATE_END);
    lip_write_int(f, dp->state_table.end_state_idx);

    lip_write_cstr(f, KEY_STATE_SPAN);
    lip_write_1darray_size_type(f, nstates, LIP_1DARRAY_UINT16);
    for (unsigned i = 0; i < nstates; ++i)
        lip_write_1darray_int_item(f, span_zip(dp->state_table.span + i));

    return f->error ? IMM_IOERROR : IMM_OK;
}

enum imm_rc imm_dp_unpack(struct imm_dp *dp, struct lip_file *f)
{
    enum imm_rc rc = IMM_OK;
    unsigned size = 0;
    enum lip_1darray_type type = 0;
    struct imm_dp_emis *e = &dp->emis;
    struct imm_dp_trans_table *tt = &dp->trans_table;
    struct imm_dp_state_table *st = &dp->state_table;

    if (!imm_expect_map_size(f, 10)) return error(IMM_IOERROR);

    /* emission */
    ERET(!imm_expect_map_key(f, KEY_EMIS_SCORE), IMM_IOERROR);
    lip_read_1darray_size_type(f, &size, &type);
    ERET(type != XLIP_1DARRAY_FLOAT, IMM_IOERROR);
    e->score = reallocf(e->score, sizeof(*e->score) * size);
    ERET(!e->score && size > 0, IMM_IOERROR);
    lip_read_1darray_float_data(f, size, e->score);

    ERET(!imm_expect_map_key(f, KEY_EMIS_OFFSET), IMM_IOERROR);
    lip_read_1darray_size_type(f, &size, &type);
    ERET(type != LIP_1DARRAY_UINT32, IMM_IOERROR);
    e->offset = reallocf(e->offset, sizeof(*e->offset) * size);
    ERET(!e->offset && size > 0, IMM_IOERROR);
    lip_read_1darray_int_data(f, size, e->offset);

    /* trans_table */
    ERET(!imm_expect_map_key(f, KEY_TRANS_SCORE), IMM_IOERROR);
    lip_read_1darray_size_type(f, &tt->ntrans, &type);
    ERET(type != XLIP_1DARRAY_FLOAT, IMM_IOERROR);
    tt->trans = reallocf(tt->trans, sizeof(*tt->trans) * tt->ntrans);
    ERET(!tt->trans && tt->ntrans > 0, IMM_IOERROR);
    for (unsigned i = 0; i < tt->ntrans; ++i)
        lip_read_1darray_float_item(f, &tt->trans[i].score);

    ERET(!imm_expect_map_key(f, KEY_TRANS_SRC), IMM_IOERROR);
    lip_read_1darray_size_type(f, &size, &type);
    ERET(type != LIP_1DARRAY_UINT16, IMM_IOERROR);
    ERET(tt->ntrans != size, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        lip_read_1darray_int_item(f, &tt->trans[i].src);

    ERET(!imm_expect_map_key(f, KEY_TRANS_OFFSET), IMM_IOERROR);
    lip_read_1darray_size_type(f, &size, &type);
    ERET(type != LIP_1DARRAY_UINT16, IMM_IOERROR);
    tt->offset = reallocf(tt->offset, sizeof(*tt->offset) * size);
    ERET(!tt->offset && size > 0, IMM_IOERROR);
    lip_read_1darray_int_data(f, size, tt->offset);

    /* state_table */
    ERET(!imm_expect_map_key(f, KEY_STATE_IDS), IMM_IOERROR);
    lip_read_1darray_size_type(f, &st->nstates, &type);
    ERET(type != LIP_1DARRAY_UINT16, IMM_IOERROR);
    st->ids = reallocf(st->ids, sizeof(*st->ids) * st->nstates);
    ERET(!st->ids && st->nstates > 0, IMM_IOERROR);
    lip_read_1darray_int_data(f, st->nstates, st->ids);

    ERET(!imm_expect_map_key(f, KEY_STATE_START), IMM_IOERROR);
    lip_read_int(f, &dp->state_table.start.state);
    ERET(!imm_expect_map_key(f, KEY_STATE_LPROB), IMM_IOERROR);
    lip_read_float(f, &dp->state_table.start.lprob);
    ERET(!imm_expect_map_key(f, KEY_STATE_END), IMM_IOERROR);
    lip_read_int(f, &dp->state_table.end_state_idx);

    ERET(!imm_expect_map_key(f, KEY_STATE_SPAN), IMM_IOERROR);
    lip_read_1darray_size_type(f, &size, &type);
    ERET(st->nstates != size, IMM_IOERROR);
    ERET(type != LIP_1DARRAY_UINT16, IMM_IOERROR);
    st->span = reallocf(st->span, sizeof(*st->span) * size);
    ERET(!st->span && size > 0, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        uint16_t span = 0;
        lip_read_1darray_int_item(f, &span);
        ERET(!span_unzip(st->span + i, span), IMM_IOERROR);
        ERET(st->span[i].max > IMM_STATE_MAX_SEQLEN, IMM_IOERROR);
    }

    if (!f->error) return IMM_OK;

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
