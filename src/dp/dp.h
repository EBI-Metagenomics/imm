#ifndef DP_H
#define DP_H

#include "dp/args.h"

struct imm_dp;

struct imm_dp *dp_new(struct dp_args const *args);
struct imm_dp *dp_reset(struct imm_dp *dp, struct dp_args const *args);

#endif
