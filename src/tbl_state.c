#include "tbl_state.h"
#include "tbl_trans.h"
#include "uthash.h"
#include <math.h>

struct tbl_state
{
    int state_id;
    const struct nhmm_state *state;
    double start_lprob;
    struct tbl_trans *tbl_transitions;
    UT_hash_handle hh;
};

void tbl_state_create(struct tbl_state **tbl_states) { *tbl_states = NULL; }

void tbl_state_add_state(struct tbl_state **tbl_states, int state_id,
                         const struct nhmm_state *state, double start_lprob)
{
    struct tbl_state *tbl_state = malloc(sizeof(struct tbl_state));
    tbl_trans_create(&tbl_state->tbl_transitions);
    tbl_state->state_id = state_id;
    tbl_state->start_lprob = start_lprob;
    tbl_state->state = state;
    HASH_ADD_INT(*tbl_states, state_id, tbl_state);
}

int tbl_state_del_state(struct tbl_state **tbl_states, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(*tbl_states, &state_id, tbl_state);
    if (tbl_state) {
        HASH_DEL(*tbl_states, tbl_state);
        return 0;
    }
    return -1;
}

const struct tbl_state *tbl_state_find_c(const struct tbl_state *tbl_states,
                                         int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    return tbl_state;
}

struct tbl_state *tbl_state_find(struct tbl_state *tbl_states, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    return tbl_state;
}

const struct nhmm_state *tbl_state_get_state(const struct tbl_state *tbl_state)
{
    return tbl_state->state;
}

struct tbl_trans **tbl_state_get_transitions(struct tbl_state *tbl_states, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    if (tbl_state)
        return &tbl_state->tbl_transitions;
    return NULL;
}

double tbl_state_get_start_lprob(const struct tbl_state *tbl_state)
{
    return tbl_state->start_lprob;
}

void tbl_state_set_start_lprob(struct tbl_state *tbl_state, double lprob)
{
    tbl_state->start_lprob = lprob;
}

const struct tbl_state *tbl_state_next_c(const struct tbl_state *tbl_state)
{
    return tbl_state->hh.next;
}

struct tbl_state *tbl_state_next(struct tbl_state *tbl_state)
{
    return tbl_state->hh.next;
}

void tbl_state_destroy(struct tbl_state **tbl_states)
{
    struct tbl_state *tbl_state, *tmp;
    if (*tbl_states) {
        HASH_ITER(hh, *tbl_states, tbl_state, tmp)
        {
            tbl_trans_destroy(&tbl_state->tbl_transitions);
            tbl_state->tbl_transitions = NULL;
            tbl_state->state = NULL;
            tbl_state->start_lprob = NAN;
            tbl_state->state_id = -1;
            HASH_DEL(*tbl_states, tbl_state);
            free(tbl_state);
        }
    }
    *tbl_states = NULL;
}
