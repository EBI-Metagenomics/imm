#ifndef SCORE_CODE_H
#define SCORE_CODE_H

#include "imm/lprob.h"
#include "score.h"
#include <math.h>

struct score_code
{
    double a;  /** Left bound on the floating-point */
    double b;  /** Right bound on the floating-point */
    double Sf; /** Scaling for forward mapping */
    double Sr; /** Scaling for reverse mapping */
};

void score_code_init(struct score_code* score_code, double a, double b);

static inline score_t score_code_encode(struct score_code const* score_code, double x)
{
    return (score_t)(score_code->Sf * (x - score_code->a));
}

static inline double score_code_decode(struct score_code const* score_code, score_t y)
{
    if (y == SCORE_INV)
        return NAN;
    return score_code->Sr * (double)y + score_code->a;
}

#endif
