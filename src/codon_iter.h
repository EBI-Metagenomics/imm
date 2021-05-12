#ifndef CODON_ITER_H
#define CODON_ITER_H

#include "imm/codon.h"
#include "imm/generics.h"

struct codon_iter
{
    struct imm_nuclt const *nuclt;
    unsigned pos;
};

static inline struct codon_iter codon_iter_begin(struct imm_nuclt const *nuclt)
{
    return (struct codon_iter){nuclt, 0};
}

static inline struct imm_codon codon_iter_next(struct codon_iter *iter)
{
    unsigned n = imm_nuclt_size(iter->nuclt);

    struct imm_codon codon;
    codon.nuclt = iter->nuclt;
    codon.a = (iter->pos / (n * n)) % n;
    codon.b = (iter->pos / n) % n;
    codon.c = iter->pos % n;
    iter->pos++;

    return codon;
}

static inline bool codon_iter_end(struct codon_iter const iter)
{
    unsigned n = imm_nuclt_size(iter.nuclt);
    return iter.pos >= n * n * n;
}

#endif
