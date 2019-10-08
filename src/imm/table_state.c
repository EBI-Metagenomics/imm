#include "imm.h"
#include "src/imm/min.h"
#include "src/rapidstring/rapidstring.h"
#include "src/uthash/uthash.h"
#include <math.h>
#include <stdlib.h>

struct imm_table_state
{
    struct emission *emissions;
    int min_seq;
    int max_seq;
    struct imm_state *interface;
};

struct emission
{
    rapidstring seq;
    double lprob;
    UT_hash_handle hh;
};

double table_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
int table_state_min_seq(const struct imm_state *state);
int table_state_max_seq(const struct imm_state *state);

struct imm_table_state *imm_table_state_create(const char *name,
                                                       const struct imm_abc *abc)
{
    struct imm_table_state *state = malloc(sizeof(struct imm_table_state));

    struct imm_state_funcs funcs = {table_state_lprob, table_state_min_seq,
                                    table_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state;
}

struct imm_state *imm_table_state_cast(struct imm_table_state *state)
{
    return state->interface;
}

const struct imm_state *imm_table_state_cast_c(const struct imm_table_state *state)
{
    return state->interface;
}

void imm_table_state_destroy(struct imm_table_state *state)
{
    if (!state)
        return;

    imm_state_destroy(state->interface);
    state->interface = NULL;

    struct emission *emiss, *tmp;
    if (state->emissions) {
        HASH_ITER(hh, s->emissions, emiss, tmp)
        {
            rs_free(&emiss->seq);
            HASH_DEL(s->emissions, emiss);
            free(emiss);
        }
    }
    state->emissions = NULL;
}

void imm_table_state_add(struct imm_table_state *state, const char *seq, double lprob)
{
    struct emission *emiss = malloc(sizeof(struct emission));
    rs_init_w(&emiss->seq, seq);
    size_t seq_len = rs_len(&emiss->seq);
    emiss->lprob = lprob;
    HASH_ADD_STR(state->emissions, seq, emiss);
    state->min_seq = MIN(state->min_seq, seq_len);
    state->max_seq = MAX(state->max_seq, seq_len);
}

double table_state_lprob(const struct imm_state *state, const char *seq, int seq_len)
{
    struct table_state *s = state->impl;
    struct emission *emiss = NULL;

    HASH_FIND(hh, s->emissions, seq, (size_t)seq_len, emiss);

    if (emiss)
        return emiss->lprob;

    return -INFINITY;
}

int table_state_normalize(struct imm_state *state)
{
    struct table_state *s = state->impl;
    int len = (int)HASH_CNT(hh, s->emissions);
    double *lprobs = malloc(sizeof(double) * (size_t)len);

    size_t i = 0;
    struct emission *emiss, *tmp;
    HASH_ITER(hh, s->emissions, emiss, tmp)
    {
        lprobs[i] = emiss->lprob;
        ++i;
    }

    int err = log_normalize(lprobs, len);
    if (err) {
        free(lprobs);
        return err;
    }

    i = 0;
    HASH_ITER(hh, s->emissions, emiss, tmp)
    {
        emiss->lprob = lprobs[i];
        ++i;
    }

    free(lprobs);
    return 0;
}

int table_state_min_seq(const struct imm_state *state)
{
    const struct table_state *s = state->impl;
    return s->min_seq;
}

int table_state_max_seq(const struct imm_state *state)
{
    const struct table_state *s = state->impl;
    return s->max_seq;
}

void table_state_destroy(struct imm_state *state)
{
    if (!state)
        return;

    if (!state->impl)
        return;

    struct table_state *s = state->impl;

    struct emission *emiss, *tmp;
    if (s->emissions) {
        HASH_ITER(hh, s->emissions, emiss, tmp)
        {
            rs_free(&emiss->seq);
            HASH_DEL(s->emissions, emiss);
            free(emiss);
        }
    }
    s->emissions = NULL;

    free(state->impl);
    state->impl = NULL;
}
