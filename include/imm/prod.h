#ifndef IMM_PROD_H
#define IMM_PROD_H

#include "imm/float.h"
#include "imm/lprob.h"
#include "imm/path.h"

struct imm_seq;

struct imm_prod
{
    struct imm_seq const *seq;
    struct imm_path path;
    imm_float loglik;
    uint64_t mseconds;
};

static inline struct imm_prod imm_prod(void)
{
    return (struct imm_prod){NULL, imm_path(), IMM_LPROB_NAN, 0};
}

static inline void imm_prod_del(struct imm_prod *prod)
{
    if (prod)
    {
        prod->seq = NULL;
        imm_path_del(&prod->path);
        prod->mseconds = 0;
    }
}

static inline void imm_prod_reset(struct imm_prod *prod)
{
    imm_path_reset(&prod->path);
    prod->loglik = IMM_LPROB_NAN;
    prod->mseconds = 0;
}

#endif
