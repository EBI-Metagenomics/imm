#ifndef ESEQ_H
#define ESEQ_H

#include "code.h"
#include "matrix/matrix.h"

struct imm_seq;
struct imm_code;

struct eseq
{
    struct matrixu16 data;
    struct imm_code const *code;
};

void eseq_del(struct eseq const *eseq);

void eseq_reset(struct eseq *eseq, struct imm_code const *code);

static inline unsigned eseq_get(struct eseq const *eseq, unsigned pos,
                                unsigned len, unsigned min_seq)
{
    uint16_t val = matrixu16_get(&eseq->data, pos, len);
    return code_translate(eseq->code, val, min_seq);
}

static inline unsigned eseq_len(struct eseq const *eseq)
{
    return matrixu16_nrows(&eseq->data) - 1;
}

void eseq_init(struct eseq *eseq, struct imm_code const *code);

enum imm_rc eseq_setup(struct eseq *eseq, struct imm_seq const *seq);

#endif
