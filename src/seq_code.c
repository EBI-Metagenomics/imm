#include "seq_code.h"
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
    struct seq_code* seq_code = malloc(sizeof(struct seq_code));

    seq_code->min_seq = min_seq;
    seq_code->max_seq = max_seq;
    seq_code->abc = abc;

    if (max_seq == 0)
        seq_code->stride = NULL;
    else {
        seq_code->stride = malloc(sizeof(unsigned) * max_seq);
        seq_code->stride[max_seq - 1] = 1;
    }

    if (max_seq > 1) {
        for (unsigned len = max_seq - 2; 1 <= len + 1; --len)
            seq_code->stride[len] = seq_code->stride[len + 1] * imm_abc_length(abc);
    }

    seq_code->offset = malloc(sizeof(unsigned) * (max_seq - min_seq + 1));
    seq_code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len) {

        unsigned i = len - min_seq;
        seq_code->offset[i] =
            seq_code->offset[i - 1] + seq_code->stride[max_seq - (len - 1) - 1];
    }

    unsigned long ncombs = ipow(imm_abc_length(abc), max_seq);

    IMM_BUG(ncombs > UINT_MAX);
    seq_code->size = seq_code->offset[max_seq - min_seq] + (unsigned)ncombs;

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

struct eseq const* seq_code_create_eseq(struct seq_code const* seq_code,
                                        struct imm_seq const*  seq)
{
    struct eseq* eseq = malloc(sizeof(struct eseq));
    eseq->seq_code = seq_code;
    unsigned ncols = seq_code->max_seq - seq_code->min_seq + 1;
    eseq->code = matrixu_create(imm_seq_length(seq) + 1, ncols);

    for (unsigned i = 0; i <= imm_seq_length(seq); ++i) {

        for (unsigned j = 0; j < ncols; ++j) {

            unsigned length = seq_code->min_seq + j;
            if (i + length > imm_seq_length(seq))
                continue;

            IMM_SUBSEQ(subseq, seq, i, length);
            unsigned min_seq = seq_code->min_seq;
            unsigned code = seq_code_encode(seq_code, imm_subseq_cast(&subseq));
            code -= seq_code_offset(seq_code, min_seq);

            matrixu_set(eseq->code, i, j, code);
        }
    }
    return eseq;
}

struct eseq * seq_code_create_eseq2(struct seq_code const* seq_code)
{
    struct eseq* eseq = malloc(sizeof(struct eseq));
    eseq->seq_code = seq_code;
    eseq->code = matrixu_create(1, seq_code->max_seq - seq_code->min_seq + 1);
    return eseq;
}

unsigned seq_code_size(struct seq_code const* seq_code, unsigned min_seq)
{
    return seq_code->size - seq_code->offset[min_seq - seq_code->min_seq];
}

unsigned seq_code_min_seq(struct seq_code const* seq_code) { return seq_code->min_seq; }

unsigned seq_code_max_seq(struct seq_code const* seq_code) { return seq_code->max_seq; }

struct imm_abc const* seq_code_abc(struct seq_code const* seq_code) { return seq_code->abc; }

void seq_code_destroy(struct seq_code const* seq_code)
{
    free_c(seq_code->offset);
    free_c(seq_code->stride);
    free_c(seq_code);
}
