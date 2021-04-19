#ifndef HMM_H
#define HMM_H

#include "imm/imm.h"
#include <stdint.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct model_state;

struct trans
{
    uint16_t  src_state;
    uint16_t  dst_state;
    imm_float lprob;
};

#define MAX_NTRANS (1 << 13)

struct imm_hmm
{
    struct imm_abc const*     abc;
    struct model_state_table* table;
    imm_float                 start_lprob;
    uint16_t                  start_state;
    HASH_DECLARE(states_tbl, 11);
    HASH_DECLARE(trans_tbl, 11);
    uint16_t     ntransitions;
    struct trans transitions[MAX_NTRANS];
};

struct imm_abc const*            hmm_abc(struct imm_hmm const* hmm);
void                             hmm_add_mstate(struct imm_hmm* hmm, struct model_state* mstate);
struct model_state const* const* hmm_get_mstates(struct imm_hmm const* hmm, struct imm_dp const* dp);

#endif
