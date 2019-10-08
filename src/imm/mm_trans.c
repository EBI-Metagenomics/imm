#include "src/imm/mm_trans.h"
#include "src/uthash/uthash.h"
#include <math.h>

struct mm_trans
{
    int state_id;
    double lprob;
    UT_hash_handle hh;
};

void mm_trans_create(struct mm_trans **head_ptr) { *head_ptr = NULL; }

void mm_trans_destroy(struct mm_trans **head_ptr)
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

struct mm_trans *mm_trans_add(struct mm_trans **head_ptr, int state_id, double lprob)
{
    struct mm_trans *mm_trans = malloc(sizeof(struct mm_trans));
    mm_trans->state_id = state_id;
    mm_trans->lprob = lprob;
    HASH_ADD_INT(*head_ptr, state_id, mm_trans);
    return mm_trans;
}

struct mm_trans *mm_trans_find(struct mm_trans *head, int state_id)
{
    struct mm_trans *mm_trans = NULL;
    HASH_FIND_INT(head, &state_id, mm_trans);
    return mm_trans;
}

const struct mm_trans *mm_trans_find_c(const struct mm_trans *head, int state_id)
{
    const struct mm_trans *mm_trans = NULL;
    HASH_FIND_INT(head, &state_id, mm_trans);
    return mm_trans;
}

void mm_trans_set_lprob(struct mm_trans *mm_trans, double lprob) { mm_trans->lprob = lprob; }

double mm_trans_get_lprob(const struct mm_trans *mm_trans) { return mm_trans->lprob; }

struct mm_trans *mm_trans_next(struct mm_trans *mm_trans) { return mm_trans->hh.next; }

const struct mm_trans *mm_trans_next_c(const struct mm_trans *mm_trans)
{
    return mm_trans->hh.next;
}

int mm_trans_get_id(const struct mm_trans *mm_trans) { return mm_trans->state_id; }
