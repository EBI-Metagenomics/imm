#ifndef IMM_DP_H
#define IMM_DP_H

#include "imm/code.h"
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
struct imm_prod;
struct imm_seq;
struct imm_task;

struct imm_dp
{
    struct imm_code const *code;
    struct imm_dp_emis emis;
    struct imm_dp_trans_table trans_table;
    struct imm_dp_state_table state_table;
};

struct lip_file;

IMM_API void imm_dp_init(struct imm_dp *dp, struct imm_code const *code);

IMM_API void imm_dp_del(struct imm_dp *dp);

IMM_API void imm_dp_dump_state_table(struct imm_dp const *dp);

IMM_API void imm_dp_dump_path(struct imm_dp const *dp,
                              struct imm_task const *task,
                              struct imm_prod const *prod,
                              imm_state_name *callb);

IMM_API unsigned imm_dp_trans_idx(struct imm_dp *dp, unsigned src_idx,
                                  unsigned dst_idx);

IMM_API enum imm_rc imm_dp_change_trans(struct imm_dp *dp, unsigned trans_idx,
                                        imm_float lprob);

IMM_API enum imm_rc imm_dp_viterbi(struct imm_dp const *dp,
                                   struct imm_task *task,
                                   struct imm_prod *prod);

IMM_API enum imm_rc imm_dp_pack(struct imm_dp const *dp, struct lip_file *io);

IMM_API enum imm_rc imm_dp_unpack(struct imm_dp *dp, struct lip_file *io);

IMM_API imm_float imm_dp_emis_score(struct imm_dp const *dp, unsigned state_id,
                                    struct imm_seq const *seq);

IMM_API imm_float imm_dp_trans_score(struct imm_dp const *dp, unsigned src,
                                     unsigned dst);

IMM_API void imm_dp_write_dot(struct imm_dp const *dp, FILE *restrict fd,
                              imm_state_name *name);

#endif
