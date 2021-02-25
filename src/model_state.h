#ifndef MODEL_STATE_H
#define MODEL_STATE_H

#include "imm/float.h"
#include <inttypes.h>
#include <stdio.h>

struct imm_abc;
struct imm_io;
struct imm_state;
struct model_trans_table;

struct model_state
{
    imm_float                 start_lprob;
    struct imm_state const*   state;
    struct model_trans_table* mtrans_table;
};

struct model_state*                   model_state_create(struct imm_state const* state, imm_float start_lprob);
void                                  model_state_destroy(struct model_state* mstate);
struct model_trans_table*             model_state_get_mtrans_table(struct model_state const* mstate);
imm_float                             model_state_get_start(struct model_state const* mstate);
static inline struct imm_state const* model_state_get_state(struct model_state const* mstate);
void                                  model_state_set_start(struct model_state* mstate, imm_float lprob);

static inline struct imm_state const* model_state_get_state(struct model_state const* mstate) { return mstate->state; }

#endif
