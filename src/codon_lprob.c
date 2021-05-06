#include "imm/codon_lprob.h"
#include "imm/arr3d.h"

void imm_codon_lprob_init(struct imm_codon_lprob *codonp,
                          struct imm_nuclt const *nuclt)
{
    codonp->nuclt = nuclt;
    imm_arr3d_init(&codonp->lprobs, imm_nuclt_len(), imm_nuclt_len(),
                   imm_nuclt_len());
    imm_arr3d_fill(&codonp->lprobs, imm_lprob_zero());
}

void imm_codon_lprob_deinit(struct imm_codon_lprob *codonp)
{
    imm_arr3d_deinit(&codonp->lprobs);
}
