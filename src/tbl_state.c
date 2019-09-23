#include "tbl_state.h"
#include "uthash.h"

struct tbl_trans
{
    int state_id;
    double lprob;
    UT_hash_handle hh;
};

struct tbl_state
{
    int state_id;
    const struct nhmm_state *state;
    struct tbl_trans *tbl_transitions;
    UT_hash_handle hh;
};

void tbl_trans_create(struct tbl_trans **tbl_transitions) { *tbl_transitions = NULL; }

void tbl_trans_destroy(struct tbl_trans **tbl_transitions)
{
    struct tbl_trans *tbl_trans, *tmp;
    if (*tbl_transitions) {
        HASH_ITER(hh, *tbl_transitions, tbl_trans, tmp)
        {
            HASH_DEL(*tbl_transitions, tbl_trans);
            free(tbl_trans);
        }
    }
}

void tbl_trans_set_lprob(struct tbl_trans **tbl_transitions, int state_id, double lprob)
{
    struct tbl_trans *tbl_trans = NULL;
    HASH_FIND_INT(*tbl_transitions, &state_id, tbl_trans);
    if (!tbl_trans) {
        tbl_trans = malloc(sizeof(struct tbl_trans));
        tbl_trans->state_id = state_id;
        HASH_ADD_INT(*tbl_transitions, state_id, tbl_trans);
    }
    tbl_trans->lprob = lprob;
}

void tbl_state_create(struct tbl_state **tbl_states) { *tbl_states = NULL; }

void tbl_state_add_state(struct tbl_state **tbl_states, int state_id,
                         const struct nhmm_state *state)
{
    struct tbl_state *tbl_state = malloc(sizeof(struct tbl_state));
    tbl_trans_create(&tbl_state->tbl_transitions);
    tbl_state->state_id = state_id;
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

const struct nhmm_state *tbl_state_get_state(const struct tbl_state *tbl_states,
                                             int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    if (tbl_state)
        return tbl_state->state;
    return NULL;
}

struct tbl_trans **tbl_state_get_transitions(struct tbl_state *tbl_states, int state_id)
{
    struct tbl_state *tbl_state = NULL;
    HASH_FIND_INT(tbl_states, &state_id, tbl_state);
    if (tbl_state)
        return &tbl_state->tbl_transitions;
    return NULL;
}

void tbl_state_destroy(struct tbl_state **tbl_states)
{
    struct tbl_state *tbl_state, *tmp;
    if (tbl_states) {
        HASH_ITER(hh, *tbl_states, tbl_state, tmp)
        {
            tbl_trans_destroy(&tbl_state->tbl_transitions);
            tbl_state->tbl_transitions = NULL;
            tbl_state->state = NULL;
            tbl_state->state_id = -1;
            HASH_DEL(*tbl_states, tbl_state);
            free(tbl_state);
        }
    }
}
