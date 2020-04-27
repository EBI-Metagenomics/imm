#include "mstate.h"
#include "cast.h"
#include "free.h"
#include "imm/io.h"
#include "imm/state.h"
#include "io.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state.h"
#include <limits.h>
#include <math.h>
#include <string.h>

static int mstate_write(struct mstate const* mstate, FILE* stream);

struct mstate* mstate_create(struct imm_state const* state, double start_lprob)
{
    struct mstate* mstate = malloc(sizeof(struct mstate));
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

double mstate_get_start(struct mstate const* mstate) { return mstate->start_lprob; }

void mstate_set_start(struct mstate* mstate, double const lprob) { mstate->start_lprob = lprob; }

struct mtrans_table* mstate_get_mtrans_table(struct mstate const* mstate)
{
    return mstate->mtrans_table;
}
