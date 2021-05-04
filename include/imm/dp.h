#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/export.h"
#include "imm/float.h"
#include "imm/state_types.h"
#include "imm/trans.h"

struct imm_dp;
struct imm_result;
struct imm_task;

IMM_API void imm_dp_del(struct imm_dp const *dp);

IMM_API unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx,
                                  unsigned dst_idx);

IMM_API int imm_dp_viterbi(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_result *result);

#ifndef NDEBUG
IMM_API void dp_dump_state_table(struct imm_dp const *dp);
#endif

#endif
