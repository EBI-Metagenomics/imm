#ifndef IMM_LOGSUM_H
#define IMM_LOGSUM_H

#define imm__logsum_nargs(...) (sizeof((int[]){__VA_ARGS__}) / sizeof(int))

#define logsum(...)                                                            \
  imm_lprob_sum(imm__logsum_nargs(__VA_ARGS__),                                \
                (float[imm__logsum_nargs(__VA_ARGS__)]){__VA_ARGS__})

#endif
