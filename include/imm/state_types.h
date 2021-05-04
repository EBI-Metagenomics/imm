#ifndef IMM_STATE_TYPES_H
#define IMM_STATE_TYPES_H

#include "imm/float.h"
#include <stdint.h>

#define IMM_STATE_NULL_ID UINT16_MAX
#define IMM_STATE_NULL_IDX UINT16_MAX

typedef uint16_t __imm_state_id_t;
typedef uint16_t __imm_state_idx_t;
typedef uint8_t __imm_state_seqlen_t;

enum state_typeid
{
    IMM_NULL_STATE = 0,
    IMM_MUTE_STATE = 1,
    IMM_NORMAL_STATE = 2,
};

struct imm_seq;
struct imm_state;

struct imm_state_vtable
{
    void (*del)(struct imm_state const *state);
    imm_float (*lprob)(struct imm_state const *state,
                       struct imm_seq const *seq);
    enum state_typeid typeid;
    void *derived;
};

#endif
