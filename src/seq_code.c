#include "seq_code.h"
#include "cast.h"
#include "eseq.h"
#include "free.h"
#include "imath.h"
#include "imm/abc.h"
#include "imm/bug.h"
#include "imm/seq.h"
#include "imm/subseq.h"
#include "matrixu.h"
#include <limits.h>
#include <stdlib.h>

struct seq_code const* seq_code_create(struct imm_abc const* abc, unsigned min_seq,
                                       unsigned max_seq)
{
    IMM_BUG(min_seq > max_seq);
    struct seq_code* seq_code = malloc(sizeof(*seq_code));

    seq_code->min_seq = cast_u8_u(min_seq);
    seq_code->max_seq = cast_u8_u(max_seq);
    seq_code->abc = abc;

    if (max_seq == 0)
        seq_code->stride = NULL;
    else {
        seq_code->stride = malloc(sizeof(*seq_code->stride) * seq_code_stride_size(seq_code));
        seq_code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1) {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            seq_code->stride[len] = seq_code->stride[len + 1] * imm_abc_length(abc);
    }

    seq_code->offset = malloc(sizeof(*seq_code->offset) * seq_code_offset_size(seq_code));
    seq_code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len) {

        unsigned i = len - min_seq;
        seq_code->offset[i] =
            seq_code->offset[i - 1] + seq_code->stride[max_seq - (len - 1) - 1];
    }

    unsigned long ncombs = ipow(imm_abc_length(abc), max_seq);

    seq_code->size = seq_code->offset[max_seq - min_seq] + cast_u_lu(ncombs);

    return seq_code;
}

unsigned seq_code_encode(struct seq_code const* seq_code, struct imm_seq const* seq)
{
    unsigned code = seq_code->offset[imm_seq_length(seq) - seq_code->min_seq];
    for (unsigned i = 0; i < imm_seq_length(seq); ++i) {

        unsigned j = (unsigned)imm_abc_symbol_idx(seq_code->abc, imm_seq_string(seq)[i]);
        unsigned offset = seq_code->max_seq - imm_seq_length(seq);
        code += seq_code->stride[i + offset] * j;
    }

    return code;
}

struct eseq* seq_code_create_eseq(struct seq_code const* seq_code)
{
    struct eseq* eseq = malloc(sizeof(*eseq));
    eseq->seq_code = seq_code;
    eseq->code = matrixu_create(1, seq_code->max_seq - seq_code->min_seq + 1);
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

    if (fwrite(&chunk.min_seq, sizeof(chunk.min_seq), 1, stream) < 1)
        return 1;

    if (fwrite(&chunk.max_seq, sizeof(chunk.max_seq), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.offset, sizeof(*chunk.offset), seq_code_offset_size(seq_code), stream) <
        seq_code_offset_size(seq_code))
        return 1;

    if (fwrite(chunk.stride, sizeof(*chunk.stride), seq_code_stride_size(seq_code), stream) <
        seq_code_stride_size(seq_code))
        return 1;

    if (fwrite(&chunk.size, sizeof(chunk.size), 1, stream) < 1)
        return 1;

    return 0;
}
