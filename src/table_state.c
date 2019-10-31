#include "imm/imm.h"
#include "min.h"
#include "uthash.h"
#include <math.h>
#include <stdlib.h>

struct imm_table_state
{
    struct imm_state* interface;
    struct emission*  emissions;
    int               min_seq;
    int               max_seq;
};

struct emission
{
    char*          seq;
    double         lprob;
    UT_hash_handle hh;
};

static double table_state_lprob(struct imm_state const* state, char const* seq, int seq_len);
static int    table_state_min_seq(struct imm_state const* state);
static int    table_state_max_seq(struct imm_state const* state);

struct imm_table_state* imm_table_state_create(char const* name, struct imm_abc const* abc)
{
    struct imm_table_state* state = malloc(sizeof(struct imm_table_state));
    state->emissions = NULL;
    state->min_seq = 0;
    state->max_seq = 0;

    struct imm_state_funcs funcs = {table_state_lprob, table_state_min_seq,
                                    table_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

void imm_table_state_destroy(struct imm_table_state* state)
{
    if (!state) {
        imm_error("state should not be NULL");
        return;
    }

    imm_state_destroy(state->interface);
    state->interface = NULL;

    struct emission *emiss, *tmp;
    HASH_ITER(hh, state->emissions, emiss, tmp)
    {
        free(emiss->seq);
        HASH_DEL(state->emissions, emiss);
        free(emiss);
    }
    state->emissions = NULL;
    state->min_seq = 0;
    state->max_seq = 0;

    free(state);
}

void imm_table_state_add(struct imm_table_state* state, char const* seq, double lprob)
{
    struct emission* emiss = malloc(sizeof(struct emission));
    emiss->seq = strdup(seq);
    int seq_len = (int)strlen(seq);
    emiss->lprob = lprob;
    HASH_ADD_STR(state->emissions, seq, emiss);
    state->min_seq = MIN(state->min_seq, seq_len);
    state->max_seq = MAX(state->max_seq, seq_len);
}

int imm_table_state_normalize(struct imm_table_state* state)
{
    int     len = (int)HASH_CNT(hh, state->emissions);
    double* lprobs = malloc(sizeof(double) * (size_t)len);

    struct emission *emiss, *tmp;
    size_t           i = 0;
    HASH_ITER(hh, state->emissions, emiss, tmp)
    {
        lprobs[i] = emiss->lprob;
        ++i;
    }
    if (imm_lprob_normalize(lprobs, len)) {
        free(lprobs);
        return -1;
    }

    i = 0;
    HASH_ITER(hh, state->emissions, emiss, tmp)
    {
        emiss->lprob = lprobs[i];
        ++i;
    }

    free(lprobs);
    return 0;
}

static double table_state_lprob(struct imm_state const* state, char const* seq, int seq_len)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);

    struct emission* emiss = NULL;
    HASH_FIND(hh, s->emissions, seq, (size_t)seq_len, emiss);

    if (emiss)
        return emiss->lprob;

    return imm_lprob_zero();
}

static int table_state_min_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);
    return s->min_seq;
}

static int table_state_max_seq(struct imm_state const* state)
{
    struct imm_table_state const* s = imm_state_get_impl_c(state);
    return s->max_seq;
}
