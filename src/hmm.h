#ifndef HMM_H
#define HMM_H

#include "imm/imm.h"
#include <stdint.h>

struct imm_abc;
struct imm_dp;
struct imm_hmm;
struct model_state;

union state_pair
{
    uint16_t ids[2];
    uint32_t key;
};

struct trans
{
    union state_pair state_pair;
    imm_float        lprob;
    struct hnode     hnode;
};

#define MAX_NTRANS (1 << 13)

struct imm_hmm
{
    struct imm_abc const*     abc;
    struct model_state_table* table;
    imm_float                 start_lprob;
    uint16_t                  start_state;
    HASH_DECLARE(state_tbl, 11);
    HASH_DECLARE(trans_tbl, 11);
    uint16_t     ntrans;
    struct trans trans[MAX_NTRANS];
};

struct imm_abc const*            hmm_abc(struct imm_hmm const* hmm);
void                             hmm_add_mstate(struct imm_hmm* hmm, struct model_state* mstate);
struct model_state const* const* hmm_get_mstates(struct imm_hmm const* hmm, struct imm_dp const* dp);

#endif
