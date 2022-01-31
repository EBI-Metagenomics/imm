#include "imm/codon_marg.h"
#include "error.h"
#include "imm/abc.h"
#include "imm/codon_lprob.h"
#include "imm/generics.h"
#include "imm/nuclt.h"
#include "io.h"
#include "xcmp.h"
#include <assert.h>

static_assert(IMM_NUCLT_SIZE == 4, "nuclt size expected to be four");

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
    unsigned n = IMM_NUCLT_SIZE;

    struct imm_codon codon = {.nuclt = iter->nuclt,
                              .a = (iter->pos / (n * n)) % n,
                              .b = (iter->pos / n) % n,
                              .c = iter->pos % n};
    iter->pos++;

    return codon;
}

static inline bool codon_iter_end(struct codon_iter const iter)
{
    unsigned n = IMM_NUCLT_SIZE;
    return iter.pos >= n * n * n;
}

static imm_float marginalization(struct imm_codon_marg const *codonm,
                                 struct imm_codon const *codon)
{
    unsigned const symbol_idx[IMM_NUCLT_SIZE] = {0, 1, 2, 3};
    unsigned any = imm_abc_any_symbol_idx(imm_super(codonm->nuclt));
    unsigned const *arr[3];
    unsigned shape[3];
    for (unsigned i = 0; i < 3; ++i)
    {
        if (codon->idx[i] == any)
        {
            arr[i] = symbol_idx;
            shape[i] = IMM_NUCLT_SIZE;
        }
        else
        {
            arr[i] = codon->idx + i;
            shape[i] = 1;
        }
    }

    struct imm_codon t;
    t.nuclt = codon->nuclt;
    imm_float lprob = imm_lprob_zero();
    for (unsigned a = 0; a < shape[0]; ++a)
    {
        for (unsigned b = 0; b < shape[1]; ++b)
        {
            for (unsigned c = 0; c < shape[2]; ++c)
            {
                t.a = arr[0][a];
                t.b = arr[1][b];
                t.c = arr[2][c];
                lprob = imm_lprob_add(lprob, imm_codon_marg_lprob(codonm, t));
            }
        }
    }

    return lprob;
}

static void set_marginal_lprobs(struct imm_codon_marg *codonm)
{
    struct imm_abc const *abc = imm_super(codonm->nuclt);
    unsigned any = imm_abc_any_symbol_idx(abc);
    assert(any == imm_nuclt_size(codonm->nuclt));

    struct imm_codon codon;
    codon.nuclt = codonm->nuclt;

    unsigned size = IMM_NUCLT_SIZE + 1;
    for (unsigned k = 0; k < 3; ++k)
    {
        for (unsigned i = 0; i < size; ++i)
        {
            for (unsigned j = 0; j < size; ++j)
            {
                codon.idx[k] = any;
                codon.idx[(k + 1) % 3] = i;
                codon.idx[(k + 2) % 3] = j;
                imm_float lprob = marginalization(codonm, &codon);
                codonm->lprobs[codon.idx[0]][codon.idx[1]][codon.idx[2]] =
                    lprob;
            }
        }
    }
}

static void set_nonmarginal_lprobs(struct imm_codon_marg *codonm,
                                   struct imm_codon_lprob const *codonp)
{
    struct codon_iter iter = codon_iter_begin(codonm->nuclt);
    while (!codon_iter_end(iter))
    {
        struct imm_codon const codon = codon_iter_next(&iter);
        imm_float lprob = imm_codon_lprob_get(codonp, codon);
        codonm->lprobs[codon.idx[0]][codon.idx[1]][codon.idx[2]] = lprob;
    }
}

struct imm_codon_marg imm_codon_marg(struct imm_codon_lprob *codonp)
{
    struct imm_codon_marg codonm;
    codonm.nuclt = codonp->nuclt;
    set_nonmarginal_lprobs(&codonm, codonp);
    set_marginal_lprobs(&codonm);
    return codonm;
}

#define CODON_SIZE                                                             \
    ((IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1) * (IMM_NUCLT_SIZE + 1))

enum imm_rc imm_codon_marg_write(struct imm_codon_marg const *codonm,
                                 FILE *file)
{
    cmp_ctx_t ctx = {0};
    xcmp_fsetup(&ctx, file);
    imm_float const *lprobs = &codonm->lprobs[0][0][0];

    cmp_write_array(&ctx, CODON_SIZE);
    for (unsigned i = 0; i < CODON_SIZE; ++i)
    {
        if (!io_write_imm_float(&ctx, lprobs[i]))
            return error(IMM_IOERROR, "failed to write imm_float");
    }
    return IMM_SUCCESS;
}

enum imm_rc imm_codon_marg_read(struct imm_codon_marg *codonm, FILE *file)
{
    cmp_ctx_t cmp = {0};
    xcmp_fsetup(&cmp, file);
    return imm_codon_marg_read_cmp(codonm, &cmp);
}

enum imm_rc imm_codon_marg_read_cmp(struct imm_codon_marg *codonm,
                                    struct cmp_ctx_s *cmp)
{
    imm_float *lprobs = &codonm->lprobs[0][0][0];

    uint32_t size = 0;
    cmp_read_array(cmp, &size);
    assert(size == CODON_SIZE);
    for (unsigned i = 0; i < CODON_SIZE; ++i)
    {
        if (!io_read_imm_float(cmp, lprobs + i))
            return error(IMM_IOERROR, "failed to read imm_float");
    }
    return IMM_SUCCESS;
}
