#include "mtrans.h"
#include "free.h"
#include "uthash.h"
#include <math.h>

struct mm_trans
{
    struct imm_state const* state;
    double                  lprob;
    UT_hash_handle          hh;
};

void mtrans_create(struct mm_trans** head_ptr) { *head_ptr = NULL; }

void mtrans_destroy(struct mm_trans** head_ptr)
{
    struct mm_trans *mm_trans, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, mm_trans, tmp)
        {
            HASH_DEL(*head_ptr, mm_trans);
            free_c(mm_trans);
        }
    }
    *head_ptr = NULL;
}

struct mm_trans* mtrans_add(struct mm_trans** head_ptr, struct imm_state const* state,
                            double lprob)
{
    struct mm_trans* mm_trans = malloc(sizeof(struct mm_trans));
    mm_trans->state = state;
    mm_trans->lprob = lprob;
    HASH_ADD_PTR(*head_ptr, state, mm_trans);
    return mm_trans;
}

void mtrans_del(struct mm_trans** head_ptr, struct mm_trans* trans)
{
    HASH_DEL(*head_ptr, trans);
    free_c(trans);
}

struct mm_trans* mtrans_find(struct mm_trans* head, struct imm_state const* state)
{
    struct mm_trans* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

struct mm_trans const* mtrans_find_c(struct mm_trans const*  head,
                                     struct imm_state const* state)
{
    struct mm_trans const* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

void mtrans_set_lprob(struct mm_trans* mm_trans, double lprob) { mm_trans->lprob = lprob; }

double mtrans_get_lprob(struct mm_trans const* mm_trans) { return mm_trans->lprob; }

struct mm_trans* mtrans_next(struct mm_trans* mm_trans) { return mm_trans->hh.next; }

struct mm_trans const* mtrans_next_c(struct mm_trans const* mm_trans)
{
    return mm_trans->hh.next;
}

struct imm_state const* mtrans_get_state(struct mm_trans const* mm_trans)
{
    return mm_trans->state;
}
