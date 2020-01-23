#ifndef IMM_MSTATE_H
#define IMM_MSTATE_H

struct imm_state;
struct mstate;
struct mtrans_table;

struct mstate*             mstate_create(struct imm_state const* state, double start_lprob);
void                       mstate_destroy(struct mstate* mstate);
struct imm_state const*    mstate_get_state(struct mstate const* mstate);
double                     mstate_get_start(struct mstate const* mstate);
void                       mstate_set_start(struct mstate* mstate, double lprob);
struct mtrans_table * mstate_get_mtrans_table(struct mstate const* mstate);
/* void                    mstate_del_trans(struct mstate* mstate, struct imm_state const*
 * state); */

/* struct mtrans*        mstate_get_trans(struct mstate* mm_state); */
/* struct mtrans const*  mstate_get_trans_c(struct mstate const* mm_state); */

/* struct mtrans**       mstate_get_trans_ptr(struct mstate* mm_state); */
/* struct mtrans* const* mstate_get_trans_ptr_c(struct mstate const* mm_state); */

#endif
