#ifndef IMM_RESULT_H
#define IMM_RESULT_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/path.h"

struct imm_seq;

struct imm_result
{
    struct imm_seq const *seq;
    struct imm_path path;
    imm_float loglik;
    uint64_t seconds;
};

static inline struct imm_result imm_result(void)
{
    return (struct imm_result){NULL, imm_path(), IMM_LPROB_NAN, 0};
}

static inline void imm_result_del(struct imm_result *result)
{
    if (result)
    {
        result->seq = NULL;
        imm_path_del(&result->path);
        result->seconds = 0;
    }
}

static inline void imm_result_reset(struct imm_result *result)
{
    imm_path_reset(&result->path);
    result->loglik = IMM_LPROB_NAN;
    result->seconds = 0;
}

#endif
