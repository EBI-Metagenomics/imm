#include "seq_code.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/bug.h"
#include "imm/seq.h"
#include <stdlib.h>

struct seq_code
{
    unsigned  min_seq;
    unsigned* offset;
    unsigned* stride;
    struct imm_abc* abc;
};

struct seq_code const* seq_code_create(struct imm_abc const* abc, unsigned min_seq,
                                       unsigned max_seq)
{
    struct seq_code* seq_code = malloc(sizeof(struct seq_code));

    seq_code->min_seq = min_seq;
    seq_code->offset = malloc(sizeof(unsigned) * (max_seq - min_seq + 1));
    seq_code->stride = malloc(sizeof(unsigned) * (max_seq - min_seq + 1));

    seq_code->offset[0] = 0;
    seq_code->stride[0] = 1;
    for (unsigned len = min_seq + 1; len <= max_seq; ++len) {

        unsigned i = len - min_seq;
        seq_code->stride[i] = seq_code->stride[i - 1] * imm_abc_length(abc);
        seq_code->offset[i] = seq_code->offset[i - 1] + seq_code->stride[i];
    }

    return seq_code;
}

unsigned seq_code_get(struct seq_code const* seq_code, struct imm_seq const* seq)
{
    IMM_BUG(seq_code->abc != imm_seq_get_abc(seq));

    unsigned code = seq_code->offset[imm_seq_length(seq)];
    for (unsigned i = 0; i < imm_seq_length(seq); ++i) {

        code += (unsigned)imm_abc_symbol_idx(seq_code->abc, imm_seq_string(seq)[i]);
    }

    return code;
}

void seq_code_destroy(struct seq_code const* seq_code)
{
    free_c(seq_code->offset);
    free_c(seq_code->stride);
    free_c(seq_code);
}
