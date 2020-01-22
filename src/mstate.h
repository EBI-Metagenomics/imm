#ifndef IMM_MSTATE_H
#define IMM_MSTATE_H

#include "hide.h"

struct imm_state;
struct mstate;

HIDE struct mstate* imm_mstate_create(struct imm_state const* state, double start_lprob);
HIDE void           imm_mstate_destroy(struct mstate* mstate);
HIDE struct imm_state const* imm_mstate_get_state(struct mstate const* mstate);
HIDE double                  imm_mstate_get_start(struct mstate const* mstate);
HIDE void                    imm_mstate_set_start(struct mstate* mstate, double lprob);
HIDE void imm_mstate_del_trans(struct mstate* mstate, struct imm_state const* state);
HIDE struct mm_trans*       imm_mstate_get_trans(struct mstate* mm_state);
HIDE struct mm_trans const* imm_mstate_get_trans_c(struct mstate const* mm_state);

HIDE struct mm_trans**       imm_mstate_get_trans_ptr(struct mstate* mm_state);
HIDE struct mm_trans* const* imm_mstate_get_trans_ptr_c(struct mstate const* mm_state);

#endif
