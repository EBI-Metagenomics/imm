#ifndef MSTATE_H
#define MSTATE_H

#include "imm/float.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_io;
struct imm_state;
struct mtrans_table;

struct mstate
{
    imm_float               start_lprob;
    struct imm_state const* state;
    struct mtrans_table*    mtrans_table;
};

struct mstate*       mstate_create(struct imm_state const* state, imm_float start_lprob);
void                 mstate_destroy(struct mstate* mstate);
struct mtrans_table* mstate_get_mtrans_table(struct mstate const* mstate);
imm_float            mstate_get_start(struct mstate const* mstate);
static inline struct imm_state const* mstate_get_state(struct mstate const* mstate);
void                                  mstate_set_start(struct mstate* mstate, imm_float lprob);

static inline struct imm_state const* mstate_get_state(struct mstate const* mstate)
{
    return mstate->state;
}

#endif
