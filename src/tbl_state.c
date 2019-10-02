#include "tbl_state.h"
#include "tbl_trans.h"
#include "uthash.h"
#include <math.h>

struct tbl_state
{
    int state_id;
    const struct imm_state *state;
    double start_lprob;
    struct tbl_trans *tbl_transitions;
    UT_hash_handle hh;
};

void tbl_state_create(struct tbl_state **head_ptr) { *head_ptr = NULL; }

void tbl_state_destroy(struct tbl_state **head_ptr)
{
    struct tbl_state *tbl_state, *tmp;
    if (*head_ptr) {
        HASH_ITER(hh, *head_ptr, tbl_state, tmp)
        {
            tbl_trans_destroy(&tbl_state->tbl_transitions);
            tbl_state->tbl_transitions = NULL;
            tbl_state->state = NULL;
            tbl_state->start_lprob = NAN;
            tbl_state->state_id = -1;
            HASH_DEL(*head_ptr, tbl_state);
            free(tbl_state);
        }
    }
    *head_ptr = NULL;
}

void tbl_state_add_state(struct tbl_state **head_ptr, int state_id,
                         const struct imm_state *state, double start_lprob)
{
    struct tbl_state *tbl_state = malloc(sizeof(struct tbl_state));
    tbl_trans_create(&tbl_state->tbl_transitions);
    tbl_state->state_id = state_id;
    tbl_state->start_lprob = start_lprob;
    tbl_state->state = state;
    HASH_ADD_INT(*head_ptr, state_id, tbl_state);
}

int tbl_state_del_state(struct tbl_state **head_ptr, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(*head_ptr, &state_id, tbl_state);
    if (tbl_state) {
        HASH_DEL(*head_ptr, tbl_state);
        return 0;
    }
    return -1;
}

struct tbl_state *tbl_state_find(struct tbl_state *head, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(head, &state_id, tbl_state);
    return tbl_state;
}

const struct tbl_state *tbl_state_find_c(const struct tbl_state *head,
                                         int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(head, &state_id, tbl_state);
    return tbl_state;
}

const struct imm_state *tbl_state_get_state(const struct tbl_state *tbl_state)
{
    return tbl_state->state;
}

double tbl_state_get_start_lprob(const struct tbl_state *tbl_state)
{
    return tbl_state->start_lprob;
}

void tbl_state_set_start_lprob(struct tbl_state *tbl_state, double lprob)
{
    tbl_state->start_lprob = lprob;
}

struct tbl_state *tbl_state_next(struct tbl_state *tbl_state)
{
    return tbl_state->hh.next;
}

const struct tbl_state *tbl_state_next_c(const struct tbl_state *tbl_state)
{
    return tbl_state->hh.next;
}

struct tbl_trans *tbl_state_get_trans(struct tbl_state *tbl_state)
{
    return *tbl_state_get_trans_ptr(tbl_state);
}

const struct tbl_trans *tbl_state_get_trans_c(const struct tbl_state *tbl_state)
{
    return *tbl_state_get_trans_ptr_c(tbl_state);
}

struct tbl_trans **tbl_state_get_trans_ptr(struct tbl_state *tbl_state)
{
    return &tbl_state->tbl_transitions;
}

struct tbl_trans *const *tbl_state_get_trans_ptr_c(const struct tbl_state *tbl_state)
{
    return &tbl_state->tbl_transitions;
}
