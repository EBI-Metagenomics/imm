#include "io.h"
#include "cmp.h"
#include "dp/imm_dp.h"
#include "imm/types.h"
#include <stdint.h>

void write_dp(unsigned nstates, FILE *file);

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

    IMM_BUG(dp.code.size > UINT16_T);
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
