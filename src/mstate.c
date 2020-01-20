#include "mstate.h"
#include "free.h"
#include "imm/imm.h"
#include "mtrans.h"
#include "uthash.h"
#include <limits.h>
#include <math.h>

struct mstate
{
    struct imm_state const* state;
    double                  start_lprob;
    struct mm_trans*        mm_transitions;
    UT_hash_handle          hh;
};

void imm_mstate_create(struct mstate** head_ptr) { *head_ptr = NULL; }

void imm_mstate_destroy(struct mstate** head_ptr)
{
    struct mstate *mm_state, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, mm_state, tmp)
        {
            imm_mtrans_destroy(&mm_state->mm_transitions);
            mm_state->mm_transitions = NULL;
            mm_state->state = NULL;
            mm_state->start_lprob = imm_lprob_invalid();
            HASH_DEL(*head_ptr, mm_state);
            free_c(mm_state);
        }
    }
    *head_ptr = NULL;
}

void imm_mstate_add_state(struct mstate** head_ptr, struct imm_state const* state,
                          double start_lprob)
{
    struct mstate* mm_state = malloc(sizeof(struct mstate));
    imm_mtrans_create(&mm_state->mm_transitions);
    mm_state->start_lprob = start_lprob;
    mm_state->state = state;
    HASH_ADD_PTR(*head_ptr, state, mm_state);
}

int imm_mstate_del_state(struct mstate** head_ptr, struct imm_state const* state)
{
    struct mstate* mm_state = NULL;
    HASH_FIND_PTR(*head_ptr, &state, mm_state);
    if (mm_state) {
        HASH_DEL(*head_ptr, mm_state);
        free_c(mm_state);
        return 0;
    }
    return 1;
}

void imm_mstate_del_trans(struct mstate* head, struct imm_state const* state)
{
    struct mm_trans* trans = imm_mtrans_find(head->mm_transitions, state);
    if (trans)
        imm_mtrans_del(&head->mm_transitions, trans);
}

int imm_mstate_nitems(struct mstate const* head)
{
    size_t n = HASH_COUNT(head);
    if (n > INT_MAX) {
        imm_error("nitems greater than INT_MAX");
        return -1;
    }

    return (int)n;
}

struct mstate* imm_mstate_find(struct mstate* head, struct imm_state const* state)
{
    struct mstate* mm_state = NULL;
    HASH_FIND_PTR(head, &state, mm_state);
    return mm_state;
}

struct mstate const* imm_mstate_find_c(struct mstate const*    head,
                                       struct imm_state const* state)
{
    struct mstate* mm_state = NULL;
    HASH_FIND_PTR(head, &state, mm_state);
    return mm_state;
}

struct imm_state const* imm_mstate_get_state(struct mstate const* mm_state)
{
    return mm_state->state;
}

double imm_mstate_get_start(struct mstate const* mm_state) { return mm_state->start_lprob; }

void imm_mstate_set_start(struct mstate* mm_state, double lprob)
{
    mm_state->start_lprob = lprob;
}

struct mstate* imm_mstate_next(struct mstate* mm_state) { return mm_state->hh.next; }

struct mstate const* imm_mstate_next_c(struct mstate const* mm_state)
{
    return mm_state->hh.next;
}

struct mm_trans* imm_mstate_get_trans(struct mstate* mm_state)
{
    return *imm_mstate_get_trans_ptr(mm_state);
}

struct mm_trans const* imm_mstate_get_trans_c(struct mstate const* mm_state)
{
    return *imm_mstate_get_trans_ptr_c(mm_state);
}

struct mm_trans** imm_mstate_get_trans_ptr(struct mstate* mm_state)
{
    return &mm_state->mm_transitions;
}

struct mm_trans* const* imm_mstate_get_trans_ptr_c(struct mstate const* mm_state)
{
    return &mm_state->mm_transitions;
}
