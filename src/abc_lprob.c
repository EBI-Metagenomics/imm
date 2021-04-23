#include "imm/abc_lprob.h"
#include "common/common.h"

struct imm_abc_lprob const *imm_abc_lprob_new(struct imm_abc const *abc,
                                              imm_float const *lprobs)
{
    uint8_t len = imm_abc_len(abc);
    size_t size = sizeof(struct imm_abc_lprob) + sizeof(imm_float) * len;
    struct imm_abc_lprob *abc_lprob = xmalloc(size);
    abc_lprob->abc = abc;
    xmemcpy(abc_lprob->lprobs, lprobs, sizeof(*lprobs) * len);
    return abc_lprob;
}
