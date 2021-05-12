#include "dp/code.h"
#include "dp/eseq.h"
#include "imm/abc.h"
#include "imm/log.h"
#include "imm/seq.h"
#include "io.h"
#include "matrix/matrix.h"
#include <assert.h>

static inline unsigned calc_size(struct code const *code)
{
    unsigned long ncombs = ipow(imm_abc_size(code->abc), code->seqlen.max);
    IMM_BUG(ncombs > UINT16_MAX);
    return (unsigned)(code->offset[code->seqlen.max - code->seqlen.min] +
                      ncombs);
}

static inline unsigned offset_size(struct code const *code)
{
    return (unsigned)(code->seqlen.max - code->seqlen.min + 1);
}

static inline unsigned stride_size(struct code const *code)
{
    return code->seqlen.max;
}

void code_deinit(struct code const *code)
{
    free(code->offset);
    free(code->stride);
}

unsigned code_encode(struct code const *code, struct imm_seq const *seq)
{
    unsigned c = code->offset[imm_seq_size(seq) - code->seqlen.min];
    unsigned len = imm_seq_size(seq);
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_abc_symbol_idx(code->abc, seq->str[i]);
        unsigned offset = code->seqlen.max - len;
        c += code->stride[i + offset] * j;
    }
    return c;
}

void code_init(struct code *code, struct imm_abc const *abc, unsigned min_seq,
               unsigned max_seq)
{
    code->seqlen.min = min_seq;
    code->seqlen.max = max_seq;
    code->abc = abc;

    if (max_seq == 0)
        code->stride = NULL;
    else
    {
        code->stride = xmalloc(sizeof(*code->stride) * stride_size(code));
        code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1)
    {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            code->stride[len] =
                (uint16_t)(code->stride[len + 1] * imm_abc_size(abc));
    }

    code->offset = xmalloc(sizeof(*code->offset) * offset_size(code));
    code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len)
    {

        unsigned i = len - min_seq;
        code->offset[i] = (uint16_t)(code->offset[i - 1] +
                                     code->stride[max_seq - (len - 1) - 1]);
    }

    code->size = calc_size(code);
}

void code_reset(struct code *code, struct imm_abc const *abc, unsigned min_seq,
                unsigned max_seq)
{
    if (code->seqlen.min == min_seq && code->seqlen.max == max_seq)
    {
        if (code->abc == abc)
            return;
        code->size = calc_size(code);
        return;
    }
    code_deinit(code);
    code_init(code, abc, min_seq, max_seq);
}

struct chunk
{
    struct
    {
        uint8_t min;
        uint8_t max;
    } seqlen;
    uint16_t *offset;
    uint16_t *stride;
    uint16_t size;
};

void code_write(struct code const *code, FILE *stream)
{
    struct chunk c;

    c.seqlen.min = (uint8_t)code->seqlen.min;
    fwrite(&code->seqlen.min, sizeof(c.seqlen.min), 1, stream);

    c.seqlen.max = (uint8_t)code->seqlen.max;
    fwrite(&code->seqlen.max, sizeof(c.seqlen.max), 1, stream);

    static_assert(sizeof(*code->offset) == sizeof(*c.offset), "wrong size");
    static_assert(sizeof(*code->stride) == sizeof(*c.stride), "wrong size");

    fwrite(code->offset, sizeof(c.offset), offset_size(code), stream);
    fwrite(code->stride, sizeof(c.stride), stride_size(code), stream);

    c.size = (uint16_t)code->size;
    fwrite(&c.size, sizeof(c.size), 1, stream);
}

int code_read(struct code *code, struct imm_abc const *abc, FILE *stream)
{
    struct chunk c;

    code->abc = abc;

    fread(&c.seqlen.min, sizeof(c.seqlen.min), 1, stream);
    code->seqlen.min = c.seqlen.min;

    fread(&c.seqlen.max, sizeof(c.seqlen.max), 1, stream);
    code->seqlen.max = c.seqlen.max;

    static_assert(sizeof(*code->offset) == sizeof(*c.offset), "wrong size");
    static_assert(sizeof(*code->stride) == sizeof(*c.stride), "wrong size");

    code->offset =
        read_array_u16_flt_size(code->offset, offset_size(code), stream);

    code->stride =
        read_array_u16_flt_size(code->stride, stride_size(code), stream);

    c.size = 0;
    fread(&c.size, sizeof(c.size), 1, stream);
    code->size = c.size;

    return IMM_SUCCESS;
}
