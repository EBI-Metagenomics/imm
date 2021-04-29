#ifndef IMM_STATE_TYPES_H
#define IMM_STATE_TYPES_H

#include <stdint.h>

#define IMM_STATE_NULL_ID UINT16_MAX
#define IMM_STATE_NULL_IDX UINT16_MAX
#define IMM_STATE_NULL_TID UINT8_MAX

typedef uint16_t imm_state_id_t;
typedef uint16_t imm_state_idx_t;
typedef uint8_t imm_state_tid_t;

enum state_typeid
{
    IMM_MUTE_STATE = 0,
    IMM_NORMAL_STATE = 1,
    IMM_NULL_STATE = IMM_STATE_NULL_TID,
};

#endif
