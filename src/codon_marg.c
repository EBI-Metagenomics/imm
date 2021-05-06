#include "imm/codon_marg.h"
#include "codon_iter.h"
#include "imm/abc.h"
#include "imm/codon_lprob.h"
#include "imm/generics.h"
#include "imm/nuclt.h"

static_assert(IMM_NUCLT_NSYMBOLS == 4, "nuclt size expected to be four");

static imm_float marginalization(struct imm_codon_marg const *codonm,
                                 struct imm_codon const *codon)
{
    unsigned const symbol_idx[IMM_NUCLT_NSYMBOLS] = {0, 1, 2, 3};
    unsigned any = imm_abc_any_symbol_idx(imm_super(codonm->nuclt));
    unsigned const *arr[3];
    unsigned shape[3];
    for (unsigned i = 0; i < 3; ++i)
    {
        if (codon->idx[i] == any)
        {
            arr[i] = symbol_idx;
            shape[i] = imm_len(codonm->nuclt);
        }
        else
        {
            arr[i] = codon->idx + i;
            shape[i] = 1;
        }
    }

    struct imm_codon t = IMM_CODON_INIT(codon->nuclt);
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
                lprob = imm_lprob_add(lprob, imm_codon_marg_lprob(codonm, &t));
            }
        }
    }

    return lprob;
}

static void set_marginal_lprobs(struct imm_codon_marg *codonm)
{
    struct imm_abc const *abc = imm_super(codonm->nuclt);
    unsigned any = imm_abc_any_symbol_idx(abc);
    IMM_BUG(any != imm_len(codonm->nuclt));

    struct imm_codon codon = IMM_CODON_INIT(codonm->nuclt);

    IMM_BUG(codonm->lprobs.shape[0] != codonm->lprobs.shape[1]);
    IMM_BUG(codonm->lprobs.shape[1] != codonm->lprobs.shape[2]);

    unsigned size = codonm->lprobs.shape[0];
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
                imm_arr3d_set(&codonm->lprobs, codon.idx, lprob);
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
        imm_float lprob = imm_codon_lprob_get(codonp, &codon);
        imm_arr3d_set(&codonm->lprobs, codon.idx, lprob);
    }
}

void imm_codon_marg_init(struct imm_codon_marg *codonm,
                         struct imm_codon_lprob *codonp)
{
    codonm->nuclt = codonp->nuclt;
    unsigned size = imm_nuclt_len(codonp->nuclt) + 1;
    imm_arr3d_init(&codonm->lprobs, size, size, size);

    set_nonmarginal_lprobs(codonm, codonp);
    set_marginal_lprobs(codonm);
}

void imm_codon_marg_deinit(struct imm_codon_marg *codonm)
{
    imm_arr3d_deinit(&codonm->lprobs);
}
