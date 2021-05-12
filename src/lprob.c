#include "imm/lprob.h"
#include "support.h"

int imm_lprob_normalize(unsigned len, imm_float arr[len])
{
    imm_float lnorm = imm_lprob_sum(len, arr);
    if (!isfinite(lnorm))
        return error(IMM_ILLEGALARG, "non-finite normalization denominator");

    for (unsigned i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return IMM_SUCCESS;
}
