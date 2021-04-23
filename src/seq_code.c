#include "seq_code.h"
#include "common/common.h"
#include "eseq.h"
#include "imm/abc.h"
#include "matrix.h"

/* struct seq_code_chunk */
/* { */
/*     uint8_t min_seq; */
/*     uint8_t max_seq; */
/*     uint16_t *offset; */
/*     uint16_t *stride; */
/*     uint16_t size; */
/* }; */

static inline uint16_t calc_size(struct seq_code const *seq_code)
{
    unsigned long ncombs = ipow(imm_abc_len(seq_code->abc), seq_code->max_seq);
    BUG(ncombs > UINT16_MAX);
    return (uint16_t)(seq_code->offset[seq_code->max_seq - seq_code->min_seq] +
                      (uint16_t)ncombs);
}

static inline uint8_t offset_size(struct seq_code const *seq_code)
{
    return (uint8_t)(seq_code->max_seq - seq_code->min_seq + 1);
}

static inline uint8_t stride_size(struct seq_code const *seq_code)
{
    return seq_code->max_seq;
}

uint_fast16_t seq_code_encode(struct seq_code const *seq_code,
                              struct imm_seq const *seq)
{
    uint_fast16_t code = seq_code->offset[imm_seq_len(seq) - seq_code->min_seq];
    unsigned len = (unsigned)imm_seq_len(seq);
    for (unsigned i = 0; i < len; ++i)
    {
        unsigned j = imm_abc_symbol_idx(seq_code->abc, imm_seq_str(seq)[i]);
        unsigned offset = (unsigned)(seq_code->max_seq - len);
        code += (uint_fast16_t)(seq_code->stride[i + offset] * j);
    }

    return code;
}

struct eseq *seq_code_new_eseq(struct seq_code const *seq_code)
{
    struct eseq *eseq = xmalloc(sizeof(*eseq));
    eseq->seq_code = seq_code;
    eseq->code = matrixu16_create(
        1, (uint_fast16_t)(seq_code->max_seq - seq_code->min_seq + 1));
    if (!eseq->code)
    {
        error_explain(IMM_OUTOFMEM);
        free(eseq);
        return NULL;
    }
    return eseq;
}

struct seq_code *seq_code_new(struct imm_abc const *abc, unsigned min_seq,
                              unsigned max_seq)
{
    BUG(min_seq > max_seq);
    struct seq_code *seq_code = xmalloc(sizeof(*seq_code));

    seq_code->min_seq = (uint8_t)min_seq;
    seq_code->max_seq = (uint8_t)max_seq;
    seq_code->abc = abc;

    if (max_seq == 0)
        seq_code->stride = NULL;
    else
    {
        seq_code->stride =
            xmalloc(sizeof(*seq_code->stride) * stride_size(seq_code));
        seq_code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1)
    {
        for (unsigned len = (unsigned)(max_seq - 2); 1 <= len + 1; --len)
            seq_code->stride[len] =
                (uint16_t)(seq_code->stride[len + 1] * imm_abc_len(abc));
    }

    seq_code->offset =
        xmalloc(sizeof(*seq_code->offset) * offset_size(seq_code));
    seq_code->offset[0] = 0;
    for (unsigned len = (unsigned)(min_seq + 1); len <= max_seq; ++len)
    {

        unsigned i = len - min_seq;
        seq_code->offset[i] =
            (uint16_t)(seq_code->offset[i - 1] +
                       seq_code->stride[max_seq - (len - 1) - 1]);
    }

    seq_code->size = calc_size(seq_code);

    return seq_code;
}

struct seq_code *seq_code_reset(struct seq_code *seq_code,
                                struct imm_abc const *abc, unsigned min_seq,
                                unsigned max_seq)
{

    if (seq_code->min_seq == min_seq && seq_code->max_seq == max_seq)
    {
        if (seq_code->abc == abc)
            return seq_code;
        seq_code->size = calc_size(seq_code);
        return seq_code;
    }
    seq_code_del(seq_code);
    return seq_code_new(abc, min_seq, max_seq);
}

#if 0
int seq_code_write(struct seq_code const *seq_code, FILE *stream)
{
    struct seq_code_chunk chunk = {.min_seq = seq_code->min_seq,
                                   .max_seq = seq_code->max_seq,
                                   .offset = seq_code->offset,
                                   .stride = seq_code->stride,
                                   .size = seq_code->size};

    if (fwrite(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1)
    {
        error("could not write min_seq");
        return 1;
    }

    if (fwrite(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1)
    {
        error("could not write max_seq");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(seq_code),
               stream) < offset_size(seq_code))
    {
        error("could not write offset");
        return 1;
    }

    if (fwrite(chunk.stride, sizeof(*chunk.stride), stride_size(seq_code),
               stream) < stride_size(seq_code))
    {
        error("could not write stride");
        return 1;
    }

    if (fwrite(&chunk.size, sizeof(chunk.size), 1, stream) < 1)
    {
        error("could not write size");
        return 1;
    }

    return 0;
}

struct seq_code const *seq_code_read(FILE *stream, struct imm_abc const *abc)
{
    struct seq_code_chunk chunk = {
        .min_seq = 0, .max_seq = 0, .offset = NULL, .stride = NULL, .size = 0};

    struct seq_code *seq_code = xmalloc(sizeof(*seq_code));
    seq_code->offset = NULL;
    seq_code->stride = NULL;
    seq_code->size = 0;
    seq_code->abc = abc;

    if (fread(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1)
    {
        error("could not read min_seq");
        goto err;
    }

    if (fread(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1)
    {
        error("could not read max_seq");
        goto err;
    }

    seq_code->min_seq = chunk.min_seq;
    seq_code->max_seq = chunk.max_seq;

    chunk.offset = xmalloc(sizeof(*chunk.offset) * offset_size(seq_code));

    if (fread(chunk.offset, sizeof(*chunk.offset), offset_size(seq_code),
              stream) < offset_size(seq_code))
    {
        error("could not read offset");
        goto err;
    }

    chunk.stride = xmalloc(sizeof(*chunk.stride) * offset_size(seq_code));

    if (fread(chunk.stride, sizeof(*chunk.stride), stride_size(seq_code),
              stream) < stride_size(seq_code))
    {
        error("could not read stride");
        goto err;
    }

    if (fread(&chunk.size, sizeof(chunk.size), 1, stream) < 1)
    {
        error("could not read size");
        goto err;
    }

    seq_code->offset = chunk.offset;
    seq_code->stride = chunk.stride;
    seq_code->size = chunk.size;

    return seq_code;

err:
    if (chunk.offset)
        free(chunk.offset);

    if (chunk.stride)
        free(chunk.stride);

    free(seq_code);

    return NULL;
}
#endif
