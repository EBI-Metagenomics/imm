#ifndef IMM_MTRANS_H
#define IMM_MTRANS_H

struct imm_state;
struct mtrans;

void mtrans_create(struct mtrans** head_ptr);
void mtrans_destroy(struct mtrans** head_ptr);

struct mtrans* mtrans_add(struct mtrans** head_ptr, struct imm_state const* state,
                            double lprob);
void             mtrans_del(struct mtrans** head_ptr, struct mtrans* trans);

struct mtrans*       mtrans_find(struct mtrans* head, struct imm_state const* state);
struct mtrans const* mtrans_find_c(struct mtrans const*  head,
                                     struct imm_state const* state);

void   mtrans_set_lprob(struct mtrans* mm_trans, double lprob);
double mtrans_get_lprob(struct mtrans const* mm_trans);

struct mtrans*       mtrans_next(struct mtrans* mm_trans);
struct mtrans const* mtrans_next_c(struct mtrans const* mm_trans);

struct imm_state const* mtrans_get_state(struct mtrans const* mm_trans);

#endif
