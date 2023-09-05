#ifndef IMM_HMM_H
#define IMM_HMM_H

#include "api.h"
#include "code.h"
#include "compiler.h"
#include "pair.h"
#include "rc.h"
#include "state.h"
#include "trans.h"
#include <stdio.h>

struct imm_dp;
struct imm_hmm;
struct imm_path;
struct imm_seq;
struct imm_state;
struct imm_mute_state;

struct imm_hmm
{
  struct imm_code const *code;
  unsigned start_state_id;

  struct
  {
    unsigned size;
    CCO_HASH_DECLARE(tbl, IMM_TRANS_MAX_SIZE - 1);
  } states;

  struct
  {
    unsigned size;
    CCO_HASH_DECLARE(tbl, IMM_TRANS_MAX_SIZE - 1);
    struct imm_trans data[(1 << IMM_TRANS_MAX_SIZE)];
    struct imm_trans *avail;
  } transitions;
};

IMM_API void imm_hmm_dump(struct imm_hmm const *, FILE *restrict,
                          imm_state_name *);
IMM_API int imm_hmm_add_state(struct imm_hmm *, struct imm_state *);
IMM_API int imm_hmm_del_state(struct imm_hmm *, struct imm_state *);
IMM_API int imm_hmm_init_dp(struct imm_hmm const *,
                            struct imm_state const *end_state, struct imm_dp *);
IMM_API void imm_hmm_reset(struct imm_hmm *);

IMM_INLINE void imm_hmm_init(struct imm_hmm *x, struct imm_code const *code)
{
  x->code = code;
  imm_hmm_reset(x);
}

IMM_API int imm_hmm_reset_dp(struct imm_hmm const *,
                             struct imm_state const *end_state,
                             struct imm_dp *);

IMM_API float imm_hmm_trans(struct imm_hmm const *, struct imm_state const *src,
                            struct imm_state const *dst);

IMM_API float imm_hmm_loglik(struct imm_hmm const *, struct imm_seq const *,
                             struct imm_path const *);

IMM_API int imm_hmm_normalize_trans(struct imm_hmm const *);

IMM_API int imm_hmm_normalize_state_trans(struct imm_state *src);

IMM_API int imm_hmm_set_start(struct imm_hmm *, struct imm_mute_state const *);

IMM_API int imm_hmm_set_trans(struct imm_hmm *, struct imm_state *src,
                              struct imm_state *dst, float lprob);

#endif
