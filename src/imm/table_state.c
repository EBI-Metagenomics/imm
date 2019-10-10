#include "imm.h"
#include "src/imm/hide.h"
#include "src/imm/min.h"
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
    char *seq;
    double lprob;
    UT_hash_handle hh;
};

HIDE double table_state_lprob(const struct imm_state *state, const char *seq, int seq_len);
HIDE int table_state_min_seq(const struct imm_state *state);
HIDE int table_state_max_seq(const struct imm_state *state);

struct imm_state *imm_table_state_create(const char *name, const struct imm_abc *abc)
{
    struct imm_table_state *state = malloc(sizeof(struct imm_table_state));
    state->emissions = NULL;
    state->min_seq = 0;
    state->max_seq = 0;

    struct imm_state_funcs funcs = {table_state_lprob, table_state_min_seq,
                                    table_state_max_seq};
    state->interface = imm_state_create(name, abc, funcs, state);
    return state->interface;
}

void imm_table_state_destroy(struct imm_state *state)
{
    if (!state)
        return;

    struct imm_table_state *s = imm_state_get_impl(state);

    imm_state_destroy(state);
    s->interface = NULL;

    struct emission *emiss, *tmp;
    HASH_ITER(hh, s->emissions, emiss, tmp)
    {
        free(emiss->seq);
        HASH_DEL(s->emissions, emiss);
        free(emiss);
    }
    s->emissions = NULL;
    s->min_seq = 0;
    s->max_seq = 0;

    free(s);
}

void imm_table_state_add(struct imm_state *state, const char *seq, double lprob)
{
    struct emission *emiss = malloc(sizeof(struct emission));
    emiss->seq = strdup(seq);
    int seq_len = (int)strlen(seq);
    emiss->lprob = lprob;
    struct imm_table_state *s = imm_state_get_impl(state);
    HASH_ADD_STR(s->emissions, seq, emiss);
    s->min_seq = MIN(s->min_seq, seq_len);
    s->max_seq = MAX(s->max_seq, seq_len);
}

int imm_table_state_normalize(struct imm_state *state)
{
    struct imm_table_state *s = imm_state_get_impl(state);
    int len = (int)HASH_CNT(hh, s->emissions);
    double *lprobs = malloc(sizeof(double) * (size_t)len);

    struct emission *emiss, *tmp;
    size_t i = 0;
    HASH_ITER(hh, s->emissions, emiss, tmp)
    {
        lprobs[i] = emiss->lprob;
        ++i;
    }
    if (imm_log_normalize(lprobs, len)) {
        free(lprobs);
        return -1;
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

double table_state_lprob(const struct imm_state *state, const char *seq, int seq_len)
{
    const struct imm_table_state *s = imm_state_get_impl_c(state);

    struct emission *emiss = NULL;
    HASH_FIND(hh, s->emissions, seq, (size_t)seq_len, emiss);

    if (emiss)
        return emiss->lprob;

    return -INFINITY;
}

int table_state_min_seq(const struct imm_state *state)
{
    const struct imm_table_state *s = imm_state_get_impl_c(state);
    return s->min_seq;
}

int table_state_max_seq(const struct imm_state *state)
{
    const struct imm_table_state *s = imm_state_get_impl_c(state);
    return s->max_seq;
}
