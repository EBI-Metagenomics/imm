#ifndef IMM_RESULT_H
#define IMM_RESULT_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/path.h"

struct imm_seq;

struct imm_result
{
    struct imm_seq const *seq;
    struct imm_path path;
    imm_float loglik;
    imm_float seconds;
};

static inline void imm_result_init(struct imm_result *result)
{
    result->seq = NULL;
    imm_path_init(&result->path);
    result->loglik = imm_lprob_nan();
    result->seconds = 0;
}

static inline void imm_result_deinit(struct imm_result *result)
{
    result->seq = NULL;
    imm_path_deinit(&result->path);
    result->seconds = 0;
}

#endif
