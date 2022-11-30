#ifndef LOGSUM_H
#define LOGSUM_H

#define __logsum_nargs(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define logsum(...)                                                            \
    imm_lprob_sum(__logsum_nargs(__VA_ARGS__),                                 \
                  (imm_float[__logsum_nargs(__VA_ARGS__)]){__VA_ARGS__})

#endif
