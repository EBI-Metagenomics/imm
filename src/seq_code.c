#include "seq_code.h"
#include "cast.h"
#include "eseq.h"
#include "free.h"
#include "imath.h"
#include "imm/abc.h"
#include "imm/bug.h"
#include "imm/seq.h"
#include "imm/subseq.h"
#include "matrix.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

struct seq_code_chunk
{
    uint8_t   min_seq;
    uint8_t   max_seq;
    uint16_t* offset;
    uint16_t* stride;
    uint16_t  size;
};

static inline uint_fast8_t offset_size(struct seq_code const* seq_code);
static inline uint_fast8_t stride_size(struct seq_code const* seq_code)
{
    return seq_code->max_seq;
}

struct seq_code const* seq_code_create(struct imm_abc const* abc, uint_fast8_t min_seq,
                                       uint_fast8_t max_seq)
{
    IMM_BUG(min_seq > max_seq);
    struct seq_code* seq_code = malloc(sizeof(*seq_code));

    seq_code->min_seq = min_seq;
    seq_code->max_seq = max_seq;
    seq_code->abc = abc;

    if (max_seq == 0)
        seq_code->stride = NULL;
    else {
        seq_code->stride = malloc(sizeof(*seq_code->stride) * stride_size(seq_code));
        seq_code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1) {
        for (unsigned len = (unsigned)(max_seq - 2); 1 <= len + 1; --len)
            seq_code->stride[len] = seq_code->stride[len + 1] * imm_abc_length(abc);
    }

    seq_code->offset = malloc(sizeof(*seq_code->offset) * offset_size(seq_code));
    seq_code->offset[0] = 0;
    for (unsigned len = (unsigned)(min_seq + 1); len <= max_seq; ++len) {

        unsigned i = len - min_seq;
        seq_code->offset[i] = seq_code->offset[i - 1] + seq_code->stride[max_seq - (len - 1) - 1];
    }

    unsigned long ncombs = ipow(imm_abc_length(abc), max_seq);

    seq_code->size = seq_code->offset[max_seq - min_seq] + cast_ul_u16(ncombs);

    return seq_code;
}

uint_fast16_t seq_code_encode(struct seq_code const* seq_code, struct imm_seq const* seq)
{
    uint_fast16_t code = seq_code->offset[imm_seq_length(seq) - seq_code->min_seq];
    uint_fast8_t  len = (uint_fast8_t)imm_seq_length(seq);
    for (uint_fast8_t i = 0; i < len; ++i) {

        uint_fast8_t j = imm_abc_symbol_idx(seq_code->abc, imm_seq_string(seq)[i]);
        uint_fast8_t offset = (uint_fast8_t)(seq_code->max_seq - len);
        code += seq_code->stride[i + offset] * j;
    }

    return code;
}

struct eseq* seq_code_create_eseq(struct seq_code const* seq_code)
{
    struct eseq* eseq = malloc(sizeof(*eseq));
    eseq->seq_code = seq_code;
    eseq->code = matrixu16_create(1, (uint_fast16_t)(seq_code->max_seq - seq_code->min_seq + 1));
    return eseq;
}

void seq_code_destroy(struct seq_code const* seq_code)
{
    free_c(seq_code->offset);
    free_c(seq_code->stride);
    free_c(seq_code);
}

int seq_code_write(struct seq_code const* seq_code, FILE* stream)
{
    struct seq_code_chunk chunk = {.min_seq = seq_code->min_seq,
                                   .max_seq = seq_code->max_seq,
                                   .offset = seq_code->offset,
                                   .stride = seq_code->stride,
                                   .size = seq_code->size};

    if (fwrite(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1) {
        imm_error("could not write min_seq");
        return 1;
    }

    if (fwrite(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1) {
        imm_error("could not write max_seq");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(seq_code), stream) <
        offset_size(seq_code)) {
        imm_error("could not write offset");
        return 1;
    }

    if (fwrite(chunk.stride, sizeof(*chunk.stride), stride_size(seq_code), stream) <
        stride_size(seq_code)) {
        imm_error("could not write stride");
        return 1;
    }

    if (fwrite(&chunk.size, sizeof(chunk.size), 1, stream) < 1) {
        imm_error("could not write size");
        return 1;
    }

    return 0;
}

struct seq_code const* seq_code_read(FILE* stream, struct imm_abc const* abc)
{
    struct seq_code_chunk chunk = {
        .min_seq = 0, .max_seq = 0, .offset = NULL, .stride = NULL, .size = 0};

    struct seq_code* seq_code = malloc(sizeof(*seq_code));
    seq_code->offset = NULL;
    seq_code->stride = NULL;
    seq_code->size = 0;
    seq_code->abc = abc;

    if (fread(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1) {
        imm_error("could not read min_seq");
        goto err;
    }

    if (fread(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1) {
        imm_error("could not read max_seq");
        goto err;
    }

    seq_code->min_seq = chunk.min_seq;
    seq_code->max_seq = chunk.max_seq;

    chunk.offset = malloc(sizeof(*chunk.offset) * offset_size(seq_code));

    if (fread(chunk.offset, sizeof(*chunk.offset), offset_size(seq_code), stream) <
        offset_size(seq_code)) {
        imm_error("could not read offset");
        goto err;
    }

    chunk.stride = malloc(sizeof(*chunk.stride) * offset_size(seq_code));

    if (fread(chunk.stride, sizeof(*chunk.stride), stride_size(seq_code), stream) <
        stride_size(seq_code)) {
        imm_error("could not read stride");
        goto err;
    }

    if (fread(&chunk.size, sizeof(chunk.size), 1, stream) < 1) {
        imm_error("could not read size");
        goto err;
    }

    seq_code->offset = chunk.offset;
    seq_code->stride = chunk.stride;
    seq_code->size = chunk.size;

    return seq_code;

err:
    if (chunk.offset)
        free_c(chunk.offset);

    if (chunk.stride)
        free_c(chunk.stride);

    free_c(seq_code);

    return NULL;
}

static inline uint_fast8_t offset_size(struct seq_code const* seq_code)
{
    return (uint_fast8_t)(seq_code->max_seq - seq_code->min_seq + 1);
}
