#ifndef IMM_ABCT_H
#define IMM_ABCT_H

#include "imm/api.h"

struct imm_abc;
struct imm_abct;

IMM_API struct imm_abct const* imm_abct_create(struct imm_abc const* abc,
                                               double const*         lprobs);
IMM_API double                 imm_abct_lprob(struct imm_abct const* abct, char symbol);
IMM_API void                   imm_abct_destroy(struct imm_abct const* abct);
IMM_API struct imm_abc const*  imm_abct_get_abc(struct imm_abct const* abct);

#endif
