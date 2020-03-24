#include "imm/seq_code.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/bug.h"
#include "imm/seq.h"
#include "ipow.h"
#include <limits.h>
#include <stdlib.h>

struct seq_code
{
    unsigned              min_seq;
    unsigned              max_seq;
    unsigned*             offset;
    unsigned*             stride;
    unsigned              size;
    struct imm_abc const* abc;
};

struct seq_code const* imm_seq_code_create(struct imm_abc const* abc, unsigned min_seq,
                                           unsigned max_seq)
{
    IMM_BUG(min_seq > max_seq);
    struct seq_code* seq_code = malloc(sizeof(struct seq_code));

    seq_code->min_seq = min_seq;
    seq_code->max_seq = max_seq;
    seq_code->abc = abc;

    seq_code->stride = malloc(sizeof(unsigned) * max_seq);
#if 0
    seq_code->stride[0] = 1;
    for (unsigned len = 1; len < max_seq; ++len) {

        seq_code->stride[len] = seq_code->stride[len - 1] * imm_abc_length(abc);
    }
#endif

    seq_code->stride[max_seq - 1] = 1;
    for (unsigned len = max_seq - 2; 1 <= len + 1; --len) {

        seq_code->stride[len] = seq_code->stride[len + 1] * imm_abc_length(abc);
    }

    seq_code->offset = malloc(sizeof(unsigned) * (max_seq - min_seq + 1));
    seq_code->offset[0] = 0;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len) {

        unsigned i = len - min_seq;
        /* seq_code->offset[i] = seq_code->offset[i - 1] + seq_code->stride[len - 1]; */
        seq_code->offset[i] =
            seq_code->offset[i - 1] + seq_code->stride[max_seq - (len - 1) - 1];
    }

    unsigned long ncombs = imm_ipow(imm_abc_length(abc), max_seq);
    IMM_BUG(ncombs > UINT_MAX);
    seq_code->size = seq_code->offset[max_seq] + (unsigned)ncombs;

    return seq_code;
}

unsigned imm_seq_code_encode(struct seq_code const* seq_code, unsigned min_seq,
                             struct imm_seq const* seq)
{
    IMM_BUG(seq_code->abc != imm_seq_get_abc(seq));
    IMM_BUG(seq_code->min_seq > min_seq);
    IMM_BUG(seq_code->max_seq < min_seq);

    unsigned code = seq_code->offset[imm_seq_length(seq) - seq_code->min_seq];
    for (unsigned i = 0; i < imm_seq_length(seq); ++i) {

        unsigned j = (unsigned)imm_abc_symbol_idx(seq_code->abc, imm_seq_string(seq)[i]);
        code += seq_code->stride[i + (seq_code->max_seq - imm_seq_length(seq))] * j;
    }

    return code - seq_code->offset[min_seq - seq_code->min_seq];
}

unsigned imm_seq_code_size(struct seq_code const* seq_code, unsigned min_seq)
{
    return seq_code->size - seq_code->offset[min_seq - seq_code->min_seq];
}

struct imm_abc const* imm_seq_code_abc(struct seq_code const* seq_code)
{
    return seq_code->abc;
}

void imm_seq_code_destroy(struct seq_code const* seq_code)
{
    imm_free(seq_code->offset);
    imm_free(seq_code->stride);
    imm_free(seq_code);
}
