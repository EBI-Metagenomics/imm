#ifndef IMM_DP_H
#define IMM_DP_H

#include "api.h"
#include "code.h"
#include "emis.h"
#include "float.h"
#include "state_table.h"
#include "trans.h"
#include "trans_table.h"
#include <stdio.h>

struct imm_abc;
struct imm_prod;
struct imm_seq;
struct imm_task;
struct imm_dp_cfg;

struct imm_dp
{
  struct imm_code const *code;
  struct imm_emis emis;
  struct imm_trans_table trans_table;
  struct imm_state_table state_table;
};

struct lip_file;

IMM_API void imm_dp_init(struct imm_dp *, struct imm_code const *);
IMM_API int imm_dp_reset(struct imm_dp *, struct imm_dp_cfg const *);

IMM_API void imm_dp_del(struct imm_dp *);

IMM_API void imm_dp_dump(struct imm_dp const *, imm_state_name *,
                         FILE *restrict);

IMM_API void imm_dp_dump_path(struct imm_dp const *,
                              struct imm_task const *task,
                              struct imm_prod const *prod,
                              struct imm_seq const *, imm_state_name *callb);

IMM_API unsigned imm_dp_nstates(struct imm_dp const *);

IMM_API unsigned imm_dp_trans_idx(struct imm_dp *, unsigned src_idx,
                                  unsigned dst_idx);

IMM_API int imm_dp_change_trans(struct imm_dp *, unsigned trans_idx,
                                float lprob);

IMM_API int imm_dp_viterbi(struct imm_dp const *, struct imm_task *task,
                           struct imm_prod *prod);

IMM_API int imm_dp_pack(struct imm_dp const *, struct lip_file *);

IMM_API int imm_dp_unpack(struct imm_dp *, struct lip_file *);

IMM_API float imm_dp_emis_score(struct imm_dp const *, unsigned state_id,
                                struct imm_seq const *seq);

IMM_API float const *imm_dp_emis_table(struct imm_dp const *dp,
                                       unsigned state_id, unsigned *size);

IMM_API float imm_dp_trans_score(struct imm_dp const *, unsigned src,
                                 unsigned dst);

IMM_API void imm_dp_write_dot(struct imm_dp const *, FILE *restrict,
                              imm_state_name *);

#endif
