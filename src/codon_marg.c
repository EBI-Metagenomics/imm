#include "imm/codon_marg.h"
#include "codon_iter.h"
#include "imm/codon_lprob.h"
#include "imm/nuclt.h"

static imm_float marginalization(struct imm_codon_marg const *codonm,
                                 unsigned const *symbols,
                                 struct imm_codon const *codon)
{
    unsigned any_symbol = symbols[imm_nuclt_len() - 1];

    /* struct imm_triplet *t = imm_codon_get(codon); */
    /* char const         seq[3] = {t.a, t.b, t.c}; */
    unsigned const *seq = imm_codon_idx(codon)->data;

    unsigned const *arr[3];
    unsigned shape[3];
    for (unsigned i = 0; i < 3; ++i)
    {
        if (seq[i] == any_symbol)
        {
            arr[i] = symbols;
            shape[i] = imm_nuclt_len();
            /* shape[i] = IMM_BASE_ABC_SIZE; */
        }
        else
        {
            arr[i] = seq + i;
            shape[i] = 1;
        }
    }

    /* struct imm_codon* tmp = imm_codon_create(imm_codon_abc(codon)); */
    struct imm_codon t = IMM_CODON_INIT(codon->nuclt);
    imm_float lprob = imm_lprob_zero();
    for (unsigned a = 0; a < shape[0]; ++a)
    {
        for (unsigned b = 0; b < shape[1]; ++b)
        {
            for (unsigned c = 0; c < shape[2]; ++c)
            {
                /* t.a = arr[0][a]; */
                /* t.b = arr[1][b]; */
                /* t.c = arr[2][c]; */
                imm_codon_set(&t, IMM_TRIPLET(arr[0][a], arr[1][b], arr[2][c]));
                lprob = imm_lprob_add(lprob, imm_codon_marg_lprob(codonm, &t));
            }
        }
    }

    return lprob;
}
static inline bool not_marginal(struct imm_codon const *codon,
                                unsigned any_symbol)
{
    struct imm_triplet const *t = imm_codon_idx(codon);
    return t->a != any_symbol && t->b != any_symbol && t->c != any_symbol;
}

static inline void set_marginal_lprob(struct imm_codon_marg *codonm,
                                      struct imm_codon const *codon,
                                      imm_float lprob)
{
    imm_arr3d_set(&codonm->lprobs, codon->idx.data, lprob);
}

static void set_marginal_lprobs(struct imm_codon_marg *codonm,
                                struct imm_nuclt const *nuclt)
{
    struct imm_abc const *abc = imm_nuclt_super(nuclt);
    unsigned any_symbol = (unsigned)imm_abc_any_symbol(abc);
    unsigned const symbols[5] = {(unsigned)imm_abc_symbol(abc, 0),
                                 (unsigned)imm_abc_symbol(abc, 1),
                                 (unsigned)imm_abc_symbol(abc, 2),
                                 (unsigned)imm_abc_symbol(abc, 3), any_symbol};

    struct imm_codon codon = IMM_CODON_INIT(nuclt);

    for (unsigned i0 = 0; i0 < imm_nuclt_len(); ++i0)
    {
        for (unsigned i1 = 0; i1 < imm_nuclt_len(); ++i1)
        {
            for (unsigned i2 = 0; i2 < imm_nuclt_len(); ++i2)
            {

                /* struct imm_triplet const t = {symbols[i0], symbols[i1], */
                /*                               symbols[i2]}; */
                struct imm_triplet const t =
                    IMM_TRIPLET(symbols[i0], symbols[i1], symbols[i2]);

                imm_codon_set(&codon, t);

                if (not_marginal(&codon, any_symbol))
                    continue;

                set_marginal_lprob(codonm, &codon,
                                   marginalization(codonm, symbols, &codon));
            }
        }
    }
}

static void set_symbol_index(struct imm_codon_marg *codonm)
{
    struct imm_abc const *abc = imm_nuclt_super(codonm->nuclt);

    char const *symbols = imm_abc_symbols(abc);

    for (unsigned i = 0; i < imm_nuclt_len(); ++i)
    {
        unsigned j = (unsigned)symbols[i];
        codonm->symbol_idx[j] =
            (imm_sym_idx_t)imm_abc_symbol_idx(abc, symbols[i]);
    }

    codonm->symbol_idx[(unsigned)imm_abc_any_symbol(abc)] =
        (imm_sym_idx_t)imm_nuclt_len();
}

static int set_nonmarginal_lprobs(struct imm_codon_marg *codonm,
                                  struct imm_codon_lprob const *codonp)
{
    struct codon_iter iter = codon_iter_begin(codonm->nuclt);
    while (!codon_iter_end(iter))
    {
        struct imm_codon const codon = codon_iter_next(&iter);
        set_marginal_lprob(codonm, &codon, imm_codon_lprob_get(codonp, &codon));
    }
    return 0;
}

void imm_codon_marg_init(struct imm_codon_marg *codonm,
                         struct imm_codon_lprob *codonp,
                         struct imm_nuclt const *nuclt)
{
    set_symbol_index(codonm);

    /* codonm->lprobs = imm_array3d_create(NSYMBOLS, NSYMBOLS, NSYMBOLS); */

    /* if (set_nonmarginal_lprobs(codonm, codonp)) { */
    /*     imm_array3d_destroy(codonm->lprobs); */
    /*     free_c(codonm); */
    /*     return NULL; */
    /* } */

    set_marginal_lprobs(codonm, codonm->nuclt);
}

void imm_codon_marg_deinit(struct imm_codon_marg *codonm);
