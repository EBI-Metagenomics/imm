#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/dp/code.h"
#include "imm/dp/emis.h"
#include "imm/dp/state_table.h"
#include "imm/dp/trans_table.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/state_types.h"
#include "imm/trans.h"
#include <stdio.h>

struct imm_abc;
struct imm_result;
struct imm_task;

struct imm_dp
{
    struct imm_dp_code code;
    struct imm_dp_emis emis;
    struct imm_dp_trans_table trans_table;
    struct imm_dp_state_table state_table;
};

IMM_API void imm_dp_init(struct imm_dp *dp, struct imm_abc const *abc);

IMM_API void imm_dp_del(struct imm_dp *dp);

IMM_API unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx,
                                  unsigned dst_idx);

IMM_API enum imm_rc imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx,
                                        imm_float lprob);

IMM_API enum imm_rc imm_dp_viterbi(struct imm_dp const *dp,
                                   struct imm_task *task,
                                   struct imm_result *result);

IMM_API enum imm_rc imm_dp_write(struct imm_dp const *dp, FILE *file);

IMM_API enum imm_rc imm_dp_read(struct imm_dp *dp, FILE *file);

#ifndef NDEBUG
IMM_API void dp_dump_state_table(struct imm_dp const *dp);
#endif

#endif
