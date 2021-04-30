#ifndef RESULT_H
#define RESULT_H

#include "imm/result.h"

static inline void result_reset(struct imm_result *result)
{
    imm_path_reset(&result->path);
    result->loglik = imm_lprob_invalid();
    result->seconds = 0;
}

#endif
