#ifndef IMM_MTRANS_H
#define IMM_MTRANS_H

struct imm_state;
struct mtrans;

struct mtrans*          mtrans_create(struct imm_state const* state, double lprob);
void                    mtrans_destroy(struct mtrans const* mtrans);
struct imm_state const* mtrans_get_state(struct mtrans const* mtrans);
double                  mtrans_get_lprob(struct mtrans const* mtrans);
void                    mtrans_set_lprob(struct mtrans* mtrans, double lprob);

#endif
