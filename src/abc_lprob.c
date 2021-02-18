#include "free.h"
#include "imm/imm.h"
#include <string.h>

struct imm_abc_lprob const* imm_abc_lprob_create(struct imm_abc const* abc, imm_float const* lprobs)
{
    struct imm_abc_lprob* abc_lprob = malloc(sizeof(struct imm_abc_lprob) + sizeof(imm_float) * imm_abc_length(abc));
    abc_lprob->abc = abc;
    memcpy(abc_lprob->lprobs, lprobs, sizeof(*lprobs) * imm_abc_length(abc));
    return abc_lprob;
}

void imm_abc_lprob_destroy(struct imm_abc_lprob const* abc_lprob) { free_c(abc_lprob); }
