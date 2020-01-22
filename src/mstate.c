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
    struct mtrans*        mm_transitions;
};

struct mstate* mstate_create(struct imm_state const* state, double start_lprob)
{
    struct mstate* mstate = malloc(sizeof(struct mstate));
    mtrans_create(&mstate->mm_transitions);
    mstate->start_lprob = start_lprob;
    mstate->state = state;
    return mstate;
}

void mstate_destroy(struct mstate* mstate)
{
    mtrans_destroy(&mstate->mm_transitions);
    free_c(mstate);
}

struct imm_state const* mstate_get_state(struct mstate const* mstate) { return mstate->state; }

double mstate_get_start(struct mstate const* mstate) { return mstate->start_lprob; }

void mstate_set_start(struct mstate* mstate, double const lprob)
{
    mstate->start_lprob = lprob;
}

void mstate_del_trans(struct mstate* mstate, struct imm_state const* state)
{
    struct mtrans* trans = mtrans_find(mstate->mm_transitions, state);
    if (trans)
        mtrans_del(&mstate->mm_transitions, trans);
}

struct mtrans* mstate_get_trans(struct mstate* mm_state)
{
    return *mstate_get_trans_ptr(mm_state);
}

struct mtrans const* mstate_get_trans_c(struct mstate const* mm_state)
{
    return *mstate_get_trans_ptr_c(mm_state);
}

struct mtrans** mstate_get_trans_ptr(struct mstate* mm_state)
{
    return &mm_state->mm_transitions;
}

struct mtrans* const* mstate_get_trans_ptr_c(struct mstate const* mm_state)
{
    return &mm_state->mm_transitions;
}
