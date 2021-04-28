#include "imm/lprob.h"
#include "common/common.h"

int imm_lprob_normalize(imm_float *arr, unsigned len)
{
    imm_float lnorm = imm_lprob_sum(arr, len);
    if (!isfinite(lnorm))
        return xerror(IMM_ILLEGALARG, "non-finite normalization denominator");

    for (unsigned i = 0; i < len; ++i)
        arr[i] -= lnorm;

    return IMM_SUCCESS;
}

imm_float imm_lprob_sum(imm_float const *arr, unsigned len)
{
    imm_float r = imm_lprob_zero();
    for (unsigned i = 0; i < len; ++i)
        r = logaddexp(r, arr[i]);
    return r;
}
