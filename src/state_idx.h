#ifndef IMM_STATE_IDX_H
#define IMM_STATE_IDX_H

#include "hide.h"
#include "khash_ptr.h"

struct imm_state;
struct state_idx;

KHASH_MAP_INIT_PTR(state_idx, struct state_idx*)

HIDE      khash_t(state_idx) * imm_state_idx_create(void);
HIDE void imm_state_idx_destroy(khash_t(state_idx) * table);
HIDE void imm_state_idx_add(khash_t(state_idx) * table, struct imm_state const* state,
                            int idx);
HIDE int  imm_state_idx_find(khash_t(state_idx) * table, struct imm_state const* state);

#endif
