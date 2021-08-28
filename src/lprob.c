#include "imm/lprob.h"
#include "imm/rnd.h"
#include "log.h"

enum imm_rc imm_lprob_normalize(unsigned len, imm_float arr[len])
{
    imm_float lnorm = imm_lprob_sum(len, arr);
    if (!isfinite(lnorm))
        return error(IMM_ILLEGALARG, "non-finite normalization denominator");

    for (unsigned i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return IMM_SUCCESS;
}

void imm_lprob_sample(struct imm_rnd *rnd, unsigned len, imm_float lprobs[len])
{
    for (unsigned i = 0; i < len; ++i)
        lprobs[i] = imm_log(imm_rnd_dbl(rnd));
}
