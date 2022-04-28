#ifndef IMM_HMM_H
#define IMM_HMM_H

#include "imm/code.h"
#include "imm/export.h"
#include "imm/float.h"
#include "imm/pair.h"
#include "imm/rc.h"
#include "imm/state.h"
#include "imm/trans.h"
#include <stdio.h>

struct imm_dp;
struct imm_hmm;
struct imm_path;
struct imm_seq;
struct imm_state;

struct imm_hmm
{
    struct imm_code const *code;

    struct
    {
        imm_float lprob;
        unsigned state_id;
    } start;

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
    } transitions;
};

IMM_API void imm_hmm_write_dot(struct imm_hmm const *hmm, FILE *restrict fd,
                               imm_state_name *name);

IMM_API enum imm_rc imm_hmm_add_state(struct imm_hmm *hmm,
                                      struct imm_state *state);

IMM_API enum imm_rc imm_hmm_init_dp(struct imm_hmm const *hmm,
                                    struct imm_state const *end_state,
                                    struct imm_dp *dp);
IMM_API void imm_hmm_reset(struct imm_hmm *hmm);

static inline void imm_hmm_init(struct imm_hmm *hmm,
                                struct imm_code const *code)
{
    hmm->code = code;
    imm_hmm_reset(hmm);
}

IMM_API enum imm_rc imm_hmm_reset_dp(struct imm_hmm const *hmm,
                                     struct imm_state const *end_state,
                                     struct imm_dp *dp);

IMM_API imm_float imm_hmm_start_lprob(struct imm_hmm const *hmm);

IMM_API imm_float imm_hmm_trans(struct imm_hmm const *hmm,
                                struct imm_state const *src,
                                struct imm_state const *dst);

IMM_API imm_float imm_hmm_loglik(struct imm_hmm const *hmm,
                                 struct imm_seq const *seq,
                                 struct imm_path const *path);

IMM_API enum imm_rc imm_hmm_normalize_trans(struct imm_hmm const *hmm);

IMM_API enum imm_rc imm_hmm_normalize_state_trans(struct imm_state *src);

IMM_API enum imm_rc imm_hmm_set_start(struct imm_hmm *hmm,
                                      struct imm_state const *state,
                                      imm_float lprob);

IMM_API enum imm_rc imm_hmm_set_trans(struct imm_hmm *hmm,
                                      struct imm_state *src,
                                      struct imm_state *dst, imm_float lprob);

#endif
