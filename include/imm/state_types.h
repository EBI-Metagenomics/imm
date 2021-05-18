#ifndef IMM_STATE_TYPES_H
#define IMM_STATE_TYPES_H

#include "imm/float.h"
#include "imm/types.h"
#include <stdint.h>

enum imm_state_typeid
{
    IMM_NULL_STATE = 0,
    IMM_MUTE_STATE = 1,
    IMM_NORMAL_STATE = 2,
    IMM_CODON_STATE = 3,
    IMM_FRAME_STATE = 4,
};

struct imm_seq;
struct imm_state;

struct imm_state_vtable
{
    void (*del)(struct imm_state const *state);
    imm_float (*lprob)(struct imm_state const *state,
                       struct imm_seq const *seq);
    enum imm_state_typeid typeid;
    void *derived;
};

#endif
