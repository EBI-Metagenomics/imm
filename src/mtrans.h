#ifndef MTRANS_H
#define MTRANS_H

#include "imm/float.h"

struct imm_state;
struct mtrans;

struct mtrans*          mtrans_create(struct imm_state const* state, imm_float lprob);
void                    mtrans_destroy(struct mtrans const* mtrans);
imm_float               mtrans_get_lprob(struct mtrans const* mtrans);
struct imm_state const* mtrans_get_state(struct mtrans const* mtrans);
void                    mtrans_set_lprob(struct mtrans* mtrans, imm_float lprob);

#endif
