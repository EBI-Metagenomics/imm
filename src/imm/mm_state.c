#include "src/imm/mm_state.h"
#include "imm.h"
#include "src/imm/mm_trans.h"
#include "src/uthash/uthash.h"
#include <limits.h>
#include <math.h>

struct mm_state
{
    const struct imm_state* state;
    double                  start_lprob;
    struct mm_trans*        mm_transitions;
    UT_hash_handle          hh;
};

void mm_state_create(struct mm_state** head_ptr) { *head_ptr = NULL; }

void mm_state_destroy(struct mm_state** head_ptr)
{
    struct mm_state *mm_state, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, mm_state, tmp)
        {
            mm_trans_destroy(&mm_state->mm_transitions);
            mm_state->mm_transitions = NULL;
            mm_state->state = NULL;
            mm_state->start_lprob = imm_lprob_invalid();
            HASH_DEL(*head_ptr, mm_state);
            free(mm_state);
        }
    }
    *head_ptr = NULL;
}

void mm_state_add_state(struct mm_state** head_ptr, const struct imm_state* state,
                        double start_lprob)
{
    struct mm_state* mm_state = malloc(sizeof(struct mm_state));
    mm_trans_create(&mm_state->mm_transitions);
    mm_state->start_lprob = start_lprob;
    mm_state->state = state;
    HASH_ADD_PTR(*head_ptr, state, mm_state);
}

int mm_state_del_state(struct mm_state** head_ptr, const struct imm_state* state)
{
    struct mm_state* mm_state = NULL;
    HASH_FIND_PTR(*head_ptr, &state, mm_state);
    if (mm_state) {
        HASH_DEL(*head_ptr, mm_state);
        free(mm_state);
        return 0;
    }
    return 1;
}

void mm_state_del_trans(struct mm_state* head, const struct imm_state* state)
{
    struct mm_trans* trans = mm_trans_find(head->mm_transitions, state);
    if (trans)
        mm_trans_del(&head->mm_transitions, trans);
}

int mm_state_nitems(const struct mm_state* head)
{
    size_t n = HASH_COUNT(head);
    if (n > INT_MAX) {
        imm_error("nitems greater than INT_MAX");
        return -1;
    }

    return (int)n;
}

struct mm_state* mm_state_find(struct mm_state* head, const struct imm_state* state)
{
    struct mm_state* mm_state = NULL;
    HASH_FIND_PTR(head, &state, mm_state);
    return mm_state;
}

const struct mm_state* mm_state_find_c(const struct mm_state*  head,
                                       const struct imm_state* state)
{
    struct mm_state* mm_state = NULL;
    HASH_FIND_PTR(head, &state, mm_state);
    return mm_state;
}

const struct imm_state* mm_state_get_state(const struct mm_state* mm_state)
{
    return mm_state->state;
}

double mm_state_get_start_lprob(const struct mm_state* mm_state)
{
    return mm_state->start_lprob;
}

void mm_state_set_start_lprob(struct mm_state* mm_state, double lprob)
{
    mm_state->start_lprob = lprob;
}

struct mm_state* mm_state_next(struct mm_state* mm_state) { return mm_state->hh.next; }

const struct mm_state* mm_state_next_c(const struct mm_state* mm_state)
{
    return mm_state->hh.next;
}

struct mm_trans* mm_state_get_trans(struct mm_state* mm_state)
{
    return *mm_state_get_trans_ptr(mm_state);
}

const struct mm_trans* mm_state_get_trans_c(const struct mm_state* mm_state)
{
    return *mm_state_get_trans_ptr_c(mm_state);
}

struct mm_trans** mm_state_get_trans_ptr(struct mm_state* mm_state)
{
    return &mm_state->mm_transitions;
}

struct mm_trans* const* mm_state_get_trans_ptr_c(const struct mm_state* mm_state)
{
    return &mm_state->mm_transitions;
}
