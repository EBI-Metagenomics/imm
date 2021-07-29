#ifndef IMM_STATE_TYPES_H
#define IMM_STATE_TYPES_H

#include "imm/float.h"
#include <stdint.h>

typedef uint16_t imm_nstates_t;
#define IMM_NSTATES_NULL UINT16_MAX

typedef uint16_t imm_state_id_t;
#define IMM_STATE_NULL_ID UINT16_MAX

typedef uint16_t imm_state_idx_t;
#define IMM_STATE_NULL_IDX UINT16_MAX

typedef uint8_t imm_state_seqlen_t;
#define IMM_STATE_NULL_SEQLEN UINT8_MAX

#define IMM_STATE_MAX_SEQLEN 5

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
    imm_float (*lprob)(struct imm_state const *state,
                       struct imm_seq const *seq);
    enum imm_state_typeid typeid;
    void *derived;
};

#endif
