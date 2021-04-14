#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

struct imm_abc_lprob const* imm_abc_lprob_create(struct imm_abc const* abc, imm_float const* lprobs)
{
    struct imm_abc_lprob* abc_lprob = xmalloc(sizeof(struct imm_abc_lprob) + sizeof(imm_float) * imm_abc_length(abc));
    abc_lprob->abc = abc;
    xmemcpy(abc_lprob->lprobs, lprobs, sizeof(*lprobs) * imm_abc_length(abc));
    return abc_lprob;
}

void imm_abc_lprob_destroy(struct imm_abc_lprob const* abc_lprob) { free((void*)abc_lprob); }

imm_float imm_abc_lprob_get(struct imm_abc_lprob const* abc_lprob, char symbol)
{
    uint8_t idx = imm_abc_symbol_idx(abc_lprob->abc, symbol);
    if (idx == IMM_ABC_INVALID_IDX) {
        error("symbol not found");
        return imm_lprob_invalid();
    }
    return abc_lprob->lprobs[idx];
}
