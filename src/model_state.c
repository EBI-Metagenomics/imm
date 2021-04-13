#include "model_state.h"
#include "model_trans_table.h"
#include "std.h"
#include <stdlib.h>

struct model_state* model_state_create(struct imm_state const* state, imm_float start_lprob)
{
    struct model_state* mstate = xmalloc(sizeof(*mstate));
    mstate->state = state;
    mstate->start_lprob = start_lprob;
    mstate->mtrans_table = model_trans_table_create();
    return mstate;
}

void model_state_destroy(struct model_state* mstate)
{
    model_trans_table_destroy(mstate->mtrans_table);
    free(mstate);
}

struct model_trans_table* model_state_get_mtrans_table(struct model_state const* mstate)
{
    return mstate->mtrans_table;
}

imm_float model_state_get_start(struct model_state const* mstate) { return mstate->start_lprob; }

void model_state_set_start(struct model_state* mstate, imm_float const lprob) { mstate->start_lprob = lprob; }
