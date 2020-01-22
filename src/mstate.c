#include "mstate.h"
#include "free.h"
#include "imm/imm.h"
#include "mtrans.h"
#include <limits.h>
#include <math.h>

struct mstate
{
    struct imm_state const* state;
    double                  start_lprob;
    struct mm_trans*        mm_transitions;
};

struct mstate* imm_mstate_create(struct imm_state const* state, double start_lprob)
{
    struct mstate* mstate = malloc(sizeof(struct mstate));
    imm_mtrans_create(&mstate->mm_transitions);
    mstate->start_lprob = start_lprob;
    mstate->state = state;
    return mstate;
}

void imm_mstate_destroy(struct mstate* mstate)
{
    imm_mtrans_destroy(&mstate->mm_transitions);
    free_c(mstate);
}

struct imm_state const* imm_mstate_get_state(struct mstate const* mstate)
{
    return mstate->state;
}

double imm_mstate_get_start(struct mstate const* mstate) { return mstate->start_lprob; }

void imm_mstate_set_start(struct mstate* mstate, double const lprob)
{
    mstate->start_lprob = lprob;
}

void imm_mstate_del_trans(struct mstate* mstate, struct imm_state const* state)
{
    struct mm_trans* trans = imm_mtrans_find(mstate->mm_transitions, state);
    if (trans)
        imm_mtrans_del(&mstate->mm_transitions, trans);
}

struct mm_trans* imm_mstate_get_trans(struct mstate* mm_state)
{
    return *imm_mstate_get_trans_ptr(mm_state);
}

struct mm_trans const* imm_mstate_get_trans_c(struct mstate const* mm_state)
{
    return *imm_mstate_get_trans_ptr_c(mm_state);
}

struct mm_trans** imm_mstate_get_trans_ptr(struct mstate* mm_state)
{
    return &mm_state->mm_transitions;
}

struct mm_trans* const* imm_mstate_get_trans_ptr_c(struct mstate const* mm_state)
{
    return &mm_state->mm_transitions;
}
