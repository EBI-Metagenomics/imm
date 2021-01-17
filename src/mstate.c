#include "mstate.h"
#include "free.h"
#include "mtrans_table.h"

struct mstate* mstate_create(struct imm_state const* state, imm_float start_lprob)
{
    struct mstate* mstate = malloc(sizeof(*mstate));
    mstate->state = state;
    mstate->start_lprob = start_lprob;
    mstate->mtrans_table = mtrans_table_create();
    return mstate;
}

void mstate_destroy(struct mstate* mstate)
{
    mtrans_table_destroy(mstate->mtrans_table);
    free_c(mstate);
}

struct mtrans_table* mstate_get_mtrans_table(struct mstate const* mstate)
{
    return mstate->mtrans_table;
}

imm_float mstate_get_start(struct mstate const* mstate) { return mstate->start_lprob; }

void mstate_set_start(struct mstate* mstate, imm_float const lprob) { mstate->start_lprob = lprob; }
