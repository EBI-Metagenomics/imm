#include "io.h"
#include "cmp.h"
#include "dp/imm_dp.h"
#include "imm/types.h"
#include "support.h"
#include <limits.h>
#include <stdint.h>

void write_dp(unsigned nstates, FILE *file);
void read_dp(struct imm_dp *dp, struct imm_abc const *abc, unsigned nstates,
             FILE *file);

struct code_types
{
    struct
    {
        uint8_t min;
        uint8_t max;
    } seqlen;
    uint16_t offset;
    uint16_t stride;
    uint16_t size;
};

struct emission_types
{
    imm_float data;
    uint32_t offset;
};

struct trans_table_types
{
    uint16_t ntrans;
    imm_float score;
    imm_state_idx_t src;
    imm_trans_idx_t offset;
};

struct state_table_types
{
    imm_nstates_t nstates;
    imm_state_id_t ids;
    struct
    {
        imm_state_idx_t state;
        imm_float lprob;
    } start;
    imm_state_idx_t end_state;
    struct
    {
        imm_state_seqlen_t min;
        imm_state_seqlen_t max;
    } span;
};

#define write_imm_float(ctx, v)                                                \
    _Generic((v), float : cmp_write_float, double : cmp_write_double)(ctx, v)

#define read_imm_float(ctx, v)                                                 \
    _Generic((v), float * : cmp_read_float, double * : cmp_read_double)(ctx, v)

static_assert(sizeof(imm_nstates_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_seqlen_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_trans_idx_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_id_t) == sizeof(uint16_t), "wrong types");
static_assert(sizeof(imm_state_idx_t) == sizeof(uint16_t), "wrong types");

void write_dp(unsigned nstates, FILE *file)
{
    struct imm_dp dp;
    unsigned size = 0;

    cmp_ctx_t cmp = {0};
    cmp_init(&cmp, file, file_reader, file_skipper, file_writer);

    /* code */
    cmp_write_u8(&cmp, (uint8_t)dp.code.seqlen.min);
    cmp_write_u8(&cmp, (uint8_t)dp.code.seqlen.max);

    size = code_offset_size(&dp.code);
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, dp.code.offset[i]);

    size = code_stride_size(&dp.code);
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, dp.code.stride[i]);

    IMM_BUG(dp.code.size > UINT16_MAX);
    cmp_write_u16(&cmp, (uint16_t)dp.code.size);

    /* emission */
    size = emission_score_size(&dp.emission, nstates);
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        write_imm_float(&cmp, dp.emission.score[i]);

    size = emission_offset_size(nstates);
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u32(&cmp, dp.emission.offset[i]);

    /* trans_table */
    cmp_write_u16(&cmp, (uint16_t)dp.trans_table.ntrans);

    size = dp.trans_table.ntrans;
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        write_imm_float(&cmp, dp.trans_table.trans[i].score);

    size = dp.trans_table.ntrans;
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, dp.trans_table.trans[i].src);

    size = trans_table_offset_size(nstates);
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, dp.trans_table.offset[i]);

    /* state_table */
    cmp_write_u16(&cmp, (uint16_t)dp.state_table.nstates);

    size = dp.state_table.nstates;
    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, dp.state_table.ids[i]);

    cmp_write_u16(&cmp, dp.state_table.start.state);
    write_imm_float(&cmp, dp.state_table.start.lprob);
    cmp_write_u16(&cmp, (uint16_t)dp.state_table.end_state);

    cmp_write_array(&cmp, size);
    for (unsigned i = 0; i < size; ++i)
        cmp_write_u16(&cmp, span_zip(dp.state_table.span + i));
}

void read_dp(struct imm_dp *dp, struct imm_abc const *abc, unsigned nstates,
             FILE *file)
{
    uint8_t u8 = 0;
    uint16_t u16 = 0;
    uint32_t u32 = 0;
    uint32_t size = 0;
    cmp_ctx_t cmp = {0};
    cmp_init(&cmp, file, file_reader, file_skipper, file_writer);

    /* code */
    struct code *code = &dp->code;
    code->abc = abc;
    cmp_read_u8(&cmp, &u8);
    code->seqlen.min = u8;
    cmp_read_u8(&cmp, &u8);
    code->seqlen.max = u8;

    cmp_read_array(&cmp, &size);
    IMM_BUG(size > UINT16_MAX);
    code->offset = xrealloc(code->offset, sizeof(*code->offset) * size);
    for (unsigned i = 0; i < size; ++i)
        cmp_read_u16(&cmp, code->offset + i);

    cmp_read_array(&cmp, &size);
    IMM_BUG(size > UINT16_MAX);
    code->stride = xrealloc(code->stride, sizeof(*code->stride) * size);
    for (unsigned i = 0; i < size; ++i)
        cmp_read_u16(&cmp, code->stride + i);

    cmp_read_u16(&cmp, &u16);
    code->size = u16;

    /* emission */
    struct emission *e = &dp->emission;
    cmp_read_array(&cmp, &size);
    e->score = xrealloc(e->score, sizeof(*e->score) * size);
    for (unsigned i = 0; i < size; ++i)
        read_imm_float(&cmp, e->score + i);

    cmp_read_array(&cmp, &size);
    e->offset = xrealloc(e->offset, sizeof(*e->offset) * size);
    for (unsigned i = 0; i < size; ++i)
    {
        cmp_read_u32(&cmp, &u32);
        e->offset[i] = u32;
    }

    /* trans_table */
    struct trans_table *tt = &dp->trans_table;
    cmp_read_u16(&cmp, &u16);
    tt->ntrans = u16;

    cmp_read_array(&cmp, &size);
    IMM_BUG(tt->ntrans != size);
    tt->trans = xrealloc(tt->trans, sizeof(*tt->trans) * size);
    for (unsigned i = 0; i < size; ++i)
        read_imm_float(&cmp, &tt->trans[i].score);

    cmp_read_array(&cmp, &size);
    IMM_BUG(tt->ntrans != size);
    for (unsigned i = 0; i < size; ++i)
    {
        cmp_read_u16(&cmp, &u16);
        tt->trans[i].src = u16;
    }

    cmp_read_array(&cmp, &size);
    IMM_BUG(tt->ntrans != size);
    for (unsigned i = 0; i < size; ++i)
    {
        cmp_read_u16(&cmp, &u16);
        tt->offset[i] = u16;
    }

    /* state_table */
    struct state_table *st = &dp->state_table;
    cmp_read_u16(&cmp, &u16);
    st->nstates = u16;

    cmp_read_array(&cmp, &size);
    IMM_BUG(st->nstates != size);
    st->ids = xrealloc(st->ids, sizeof(*st->ids) * size);
    for (unsigned i = 0; i < size; ++i)
        cmp_read_u16(&cmp, st->ids + i);

    cmp_read_u16(&cmp, &st->start.state);
    read_imm_float(&cmp, &st->start.lprob);
    cmp_read_u16(&cmp, &u16);
    st->end_state = u16;

    cmp_write_array(&cmp, size);
    IMM_BUG(st->nstates != size);
    st->span = xrealloc(st->span, sizeof(*st->span) * nstates);
    for (unsigned i = 0; i < size; ++i)
    {
        cmp_read_u16(&cmp, &u16);
        span_unzip(st->span + i, u16);
    }
}
