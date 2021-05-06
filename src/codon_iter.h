#ifndef CODON_ITER_H
#define CODON_ITER_H

#include "imm/codon.h"

struct codon_iter
{
    struct imm_nuclt const *nuclt;
    char const *symbols;
    unsigned pos;
};

static inline struct codon_iter codon_iter_begin(struct imm_nuclt const *nuclt)
{
    char const *symbols = imm_abc_symbols(imm_nuclt_super(nuclt));
    return (struct codon_iter){nuclt, symbols, 0};
}

static inline struct imm_codon codon_iter_next(struct codon_iter *iter)
{
#if 0
    unsigned const n = imm_nuclt_nsymbols();
    unsigned a = (iter->pos / (n * n)) % n;
    unsigned b = (iter->pos / n) % n;
    unsigned c = iter->pos % n;
    iter->pos++;

    struct imm_triplet t = {iter->symbols[a], iter->symbols[b], iter->symbols[c]};
    IMM_CODON_DECL(codon, iter->base_abc);
    imm_codon_set_triplet(&codon, t);

    return codon;
#endif
}

static inline bool codon_iter_end(struct codon_iter const iter)
{
    unsigned const n = imm_nuclt_len();
    return iter.pos >= n * n * n;
}

#endif
