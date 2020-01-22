#include "mtrans.h"
#include "free.h"
#include "uthash.h"
#include <math.h>

struct mtrans
{
    struct imm_state const* state;
    double                  lprob;
    UT_hash_handle          hh;
};

void mtrans_create(struct mtrans** head_ptr) { *head_ptr = NULL; }

void mtrans_destroy(struct mtrans** head_ptr)
{
    struct mtrans *mm_trans, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, mm_trans, tmp)
        {
            HASH_DEL(*head_ptr, mm_trans);
            free_c(mm_trans);
        }
    }
    *head_ptr = NULL;
}

struct mtrans* mtrans_add(struct mtrans** head_ptr, struct imm_state const* state,
                            double lprob)
{
    struct mtrans* mm_trans = malloc(sizeof(struct mtrans));
    mm_trans->state = state;
    mm_trans->lprob = lprob;
    HASH_ADD_PTR(*head_ptr, state, mm_trans);
    return mm_trans;
}

void mtrans_del(struct mtrans** head_ptr, struct mtrans* trans)
{
    HASH_DEL(*head_ptr, trans);
    free_c(trans);
}

struct mtrans* mtrans_find(struct mtrans* head, struct imm_state const* state)
{
    struct mtrans* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

struct mtrans const* mtrans_find_c(struct mtrans const*  head,
                                     struct imm_state const* state)
{
    struct mtrans const* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

void mtrans_set_lprob(struct mtrans* mm_trans, double lprob) { mm_trans->lprob = lprob; }

double mtrans_get_lprob(struct mtrans const* mm_trans) { return mm_trans->lprob; }

struct mtrans* mtrans_next(struct mtrans* mm_trans) { return mm_trans->hh.next; }

struct mtrans const* mtrans_next_c(struct mtrans const* mm_trans)
{
    return mm_trans->hh.next;
}

struct imm_state const* mtrans_get_state(struct mtrans const* mm_trans)
{
    return mm_trans->state;
}
