#include "io.h"
#include "cmp.h"
#include "dp/imm_dp.h"
#include "imm/dp.h"
#include "support.h"

#define write_imm_float(ctx, v)                                                \
    _Generic((v), float : cmp_write_float, double : cmp_write_double)(ctx, v)

#define read_imm_float(ctx, v)                                                 \
    _Generic((v), float * : cmp_read_float, double * : cmp_read_double)(ctx, v)

static_assert(sizeof(imm_nstates_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_seqlen_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_trans_idx_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_id_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_idx_t) == sizeof(uint16_t), "wrong types");

int imm_dp_write(struct imm_dp const *dp, FILE *file)
{
    unsigned size = 0;
    unsigned nstates = dp->state_table.nstates;

    cmp_ctx_t ctx = {0};
    io_init(&ctx, file);

    /* code */
    cmp_write_u8(&ctx, (uint8_t)dp->code.seqlen.min);
    cmp_write_u8(&ctx, (uint8_t)dp->code.seqlen.max);

    size = code_offset_size(&dp->code);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->code.offset[i]);

    size = code_stride_size(&dp->code);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->code.stride[i]);

    IMM_BUG(dp->code.size > UINT16_MAX);
    cmp_write_u16(&ctx, (uint16_t)dp->code.size);

    /* emission */
    size = emission_score_size(&dp->emission, nstates);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        write_imm_float(&ctx, dp->emission.score[i]);

    size = emission_offset_size(nstates);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u32(&ctx, (uint32_t)dp->emission.offset[i]);

    /* trans_table */
    cmp_write_u16(&ctx, (uint16_t)dp->trans_table.ntrans);

    size = dp->trans_table.ntrans;
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        write_imm_float(&ctx, dp->trans_table.trans[i].score);

    size = dp->trans_table.ntrans;
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->trans_table.trans[i].src);

    size = trans_table_offsize(nstates);
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->trans_table.offset[i]);

    /* state_table */
    cmp_write_u16(&ctx, (uint16_t)dp->state_table.nstates);

    size = dp->state_table.nstates;
    cmp_write_array(&ctx, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&ctx, dp->state_table.ids[i]);

    cmp_write_u16(&ctx, dp->state_table.start.state);
    write_imm_float(&ctx, dp->state_table.start.lprob);
    cmp_write_u16(&ctx, (uint16_t)dp->state_table.end_state_idx);

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
            err = e;                                                           \
            goto cleanup;                                                      \
        }                                                                      \
    } while (0)

int imm_dp_read(struct imm_dp *dp, FILE *file)
{
    int err = IMM_SUCCESS;
    uint8_t u8 = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint32_t size = 0;
    cmp_ctx_t ctx = {0};
    io_init(&ctx, file);

    struct emission *e = NULL;
    struct trans_table *tt = NULL;
    struct state_table *st = NULL;

    /* code */
    struct code *code = &dp->code;
    ERETURN(!cmp_read_u8(&ctx, &u8), IMM_IOERROR);
    code->seqlen.min = u8;
    ERETURN(!cmp_read_u8(&ctx, &u8), IMM_IOERROR);
    code->seqlen.max = u8;
    ERETURN(code->seqlen.min > code->seqlen.min, IMM_PARSEERROR);
    ERETURN(code->seqlen.max > IMM_STATE_MAX_SEQLEN, IMM_PARSEERROR);

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(size > UINT16_MAX, IMM_PARSEERROR);
    code->offset = reallocf(code->offset, sizeof(*code->offset) * size);
    ERETURN(!code->offset, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!cmp_read_u16(&ctx, code->offset + i), IMM_IOERROR);

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(size > UINT16_MAX, IMM_PARSEERROR);
    code->stride = reallocf(code->stride, sizeof(*code->stride) * size);
    ERETURN(!code->stride, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!cmp_read_u16(&ctx, code->stride + i), IMM_IOERROR);

    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    code->size = u16;

    /* emission */
    e = &dp->emission;
    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->score = reallocf(e->score, sizeof(*e->score) * size);
    ERETURN(!e->score, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!read_imm_float(&ctx, e->score + i), IMM_IOERROR);

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    e->offset = reallocf(e->offset, sizeof(*e->offset) * size);
    ERETURN(!e->offset, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u32(&ctx, &u32), IMM_IOERROR);
        e->offset[i] = u32;
    }

    /* trans_table */
    tt = &dp->trans_table;
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    tt->ntrans = u16;

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(tt->ntrans != size, IMM_PARSEERROR);
    tt->trans = reallocf(tt->trans, sizeof(*tt->trans) * size);
    ERETURN(!tt->trans, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!read_imm_float(&ctx, &tt->trans[i].score), IMM_IOERROR);

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(tt->ntrans != size, IMM_PARSEERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        tt->trans[i].src = u16;
    }

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    tt->offset = reallocf(tt->offset, sizeof(*tt->offset) * size);
    ERETURN(!tt->offset, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        tt->offset[i] = u16;
    }

    /* state_table */
    st = &dp->state_table;
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    st->nstates = u16;

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(st->nstates != size, IMM_PARSEERROR);
    st->ids = reallocf(st->ids, sizeof(*st->ids) * size);
    ERETURN(!st->ids, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
        ERETURN(!cmp_read_u16(&ctx, st->ids + i), IMM_IOERROR);

    ERETURN(!cmp_read_u16(&ctx, &st->start.state), IMM_IOERROR);
    ERETURN(!read_imm_float(&ctx, &st->start.lprob), IMM_IOERROR);
    ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
    st->end_state_idx = u16;

    ERETURN(!cmp_read_array(&ctx, &size), IMM_IOERROR);
    ERETURN(st->nstates != size, IMM_PARSEERROR);
    st->span = reallocf(st->span, sizeof(*st->span) * size);
    ERETURN(!st->span, IMM_IOERROR);
    for (unsigned i = 0; i < size; ++i)
    {
        ERETURN(!cmp_read_u16(&ctx, &u16), IMM_IOERROR);
        ERETURN(!span_unzip(st->span + i, u16), IMM_PARSEERROR);
        ERETURN(st->span[i].max > IMM_STATE_MAX_SEQLEN, IMM_PARSEERROR);
    }

    return err;

cleanup:
    if (code)
    {
        free(code->offset);
        code->offset = NULL;
        free(code->stride);
        code->stride = NULL;
    }
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
    return err;
}
