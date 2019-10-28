#include "src/imm/mm_trans.h"
#include "src/uthash/uthash.h"
#include <math.h>

struct mm_trans
{
    struct imm_state const* state;
    double                  lprob;
    UT_hash_handle          hh;
};

void mm_trans_create(struct mm_trans** head_ptr) { *head_ptr = NULL; }

void mm_trans_destroy(struct mm_trans** head_ptr)
{
    struct mm_trans *mm_trans, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, mm_trans, tmp)
        {
            HASH_DEL(*head_ptr, mm_trans);
            free(mm_trans);
        }
    }
    *head_ptr = NULL;
}

struct mm_trans* mm_trans_add(struct mm_trans** head_ptr, struct imm_state const* state,
                              double lprob)
{
    struct mm_trans* mm_trans = malloc(sizeof(struct mm_trans));
    mm_trans->state = state;
    mm_trans->lprob = lprob;
    HASH_ADD_PTR(*head_ptr, state, mm_trans);
    return mm_trans;
}

void mm_trans_del(struct mm_trans** head_ptr, struct mm_trans* trans)
{
    HASH_DEL(*head_ptr, trans);
    free(trans);
}

struct mm_trans* mm_trans_find(struct mm_trans* head, struct imm_state const* state)
{
    struct mm_trans* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

struct mm_trans const* mm_trans_find_c(struct mm_trans const*  head,
                                       struct imm_state const* state)
{
    struct mm_trans const* mm_trans = NULL;
    HASH_FIND_PTR(head, &state, mm_trans);
    return mm_trans;
}

void mm_trans_set_lprob(struct mm_trans* mm_trans, double lprob) { mm_trans->lprob = lprob; }

double mm_trans_get_lprob(struct mm_trans const* mm_trans) { return mm_trans->lprob; }

struct mm_trans* mm_trans_next(struct mm_trans* mm_trans) { return mm_trans->hh.next; }

struct mm_trans const* mm_trans_next_c(struct mm_trans const* mm_trans)
{
    return mm_trans->hh.next;
}

struct imm_state const* mm_trans_get_state(struct mm_trans const* mm_trans)
{
    return mm_trans->state;
}
