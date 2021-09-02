#include "imm/lprob.h"
#include "error.h"
#include "imm/rnd.h"

void imm_lprob_normalize(unsigned len, imm_float arr[len])
{
    imm_float lnorm = imm_lprob_sum(len, arr);
    for (unsigned i = 0; i < len; ++i)
        arr[i] -= lnorm;
}

void imm_lprob_sample(struct imm_rnd *rnd, unsigned len, imm_float lprobs[len])
{
    for (unsigned i = 0; i < len; ++i)
        lprobs[i] = imm_log(imm_rnd_dbl(rnd));
}
