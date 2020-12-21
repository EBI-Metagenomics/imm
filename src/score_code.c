#include "score_code.h"
#include <math.h>

void score_code_init(struct score_code* score_code, double a, double b)
{
    unsigned const L = sizeof(score_t);
    double const   b_pow = ceil(log2(b - a));
    double const   d_pow = 8 * L - 1;

    score_code->a = a;
    score_code->b = b;

    score_code->Sf = pow(2.0, d_pow - b_pow);
    score_code->Sr = pow(2.0, b_pow - d_pow);
}
