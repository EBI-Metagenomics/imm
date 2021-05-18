#ifndef IMM_TYPES_H
#define IMM_TYPES_H

#include <stdint.h>

typedef uint16_t imm_nstates_t;

#define IMM_NULL_NSTATES UINT16_MAX

#define IMM_STATE_NULL_ID UINT16_MAX
#define IMM_STATE_NULL_IDX UINT16_MAX
#define IMM_STATE_NULL_SEQLEN UINT8_MAX

typedef uint16_t imm_state_id_t;
typedef uint16_t imm_state_idx_t;
typedef uint8_t imm_state_seqlen_t;

#define IMM_STATE_MAX_SEQLEN 5

#endif
