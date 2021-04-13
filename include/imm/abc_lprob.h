#ifndef IMM_ABC_LPROB_H
#define IMM_ABC_LPROB_H

#include "imm/abc.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"

struct imm_abc_lprob
{
    struct imm_abc const* abc;
    imm_float             lprobs[];
};

static inline struct imm_abc const* imm_abc_lprob_abc(struct imm_abc_lprob const* abc_lprob) { return abc_lprob->abc; }
IMM_API struct imm_abc_lprob const* imm_abc_lprob_create(struct imm_abc const* abc, imm_float const* lprobs);
IMM_API void                        imm_abc_lprob_destroy(struct imm_abc_lprob const* abc_lprob);
IMM_API imm_float                   imm_abc_lprob_get(struct imm_abc_lprob const* abc_lprob, char symbol);

#endif
