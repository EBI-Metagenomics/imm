#include "state/table.h"
#include "alphabet.h"
#include "array.h"
#include "macro.h"
#include "report.h"
#include "uthash.h"
#include <math.h>
#include <stdlib.h>

struct table_state
{
    struct emission *emissions;
    int min_seq;
    int max_seq;
};

struct emission
{
    rapidstring seq;
    double lprob;
    UT_hash_handle hh;
};

void table_state_create(struct imm_state *state)
{
    struct table_state *s = malloc(sizeof(struct table_state));
    state->impl = s;
    s->emissions = NULL;
}

void table_state_add(struct table_state *state, const char *seq, int seq_len,
                     double lprob)
{
    struct emission *emiss = malloc(sizeof(struct emission));
    rs_init_w_n(&emiss->seq, seq, (size_t)seq_len);
    emiss->lprob = lprob;
    HASH_ADD_KEYPTR(hh, state->emissions, rs_data_c(&emiss->seq), seq_len, emiss);
    state->min_seq = MIN(state->min_seq, seq_len);
    state->max_seq = MAX(state->max_seq, seq_len);
}

double table_state_emiss_lprob(const struct imm_state *state, const char *seq,
                               int seq_len)
{
    struct table_state *s = state->impl;
    struct emission *emiss = NULL;

    HASH_FIND(hh, s->emissions, seq, (size_t) seq_len, emiss);

    if (emiss)
        return emiss->lprob;

    return -INFINITY;
}

int table_state_normalize(struct imm_state *state)
{
    struct table_state *s = state->impl;
    int len = (int) HASH_CNT(hh, s->emissions);
    double *lprobs = malloc(sizeof(double) * (size_t) len);

    size_t i = 0;
    struct emission *emiss, *tmp;
    HASH_ITER(hh, s->emissions, emiss, tmp)
    {
        lprobs[i] = emiss->lprob;
        ++i;
    }

    int err = log_normalize(lprobs, len);
    if (err)
    {
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
