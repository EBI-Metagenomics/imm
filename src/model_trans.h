#ifndef MODEL_TRANS_H
#define MODEL_TRANS_H

#include "imm/float.h"

struct imm_state;
struct model_trans;

struct model_trans*     model_trans_create(struct imm_state const* state, imm_float lprob);
void                    model_trans_destroy(struct model_trans const* mtrans);
imm_float               model_trans_get_lprob(struct model_trans const* mtrans);
struct imm_state const* model_trans_get_state(struct model_trans const* mtrans);
void                    model_trans_set_lprob(struct model_trans* mtrans, imm_float lprob);

#endif
