#ifndef LOGSUM_H
#define LOGSUM_H

#define __LOGSUM_NARGS(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define LOGSUM(...)                                                            \
    imm_lprob_sum(__LOGSUM_NARGS(__VA_ARGS__),                                 \
                  (imm_float[__LOGSUM_NARGS(__VA_ARGS__)]){__VA_ARGS__})

#endif
