#define NHMM_API_EXPORTS

#include "state/state.h"
#include "alphabet.h"
#include "state/normal.h"

#include <stdlib.h>

struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *alphabet);

NHMM_API struct nhmm_state *
nhmm_state_create_normal(const char *name, const struct nhmm_alphabet *alphabet,
                         double *emission_lprobs)
{
    struct nhmm_state *s = state_create(name, alphabet);
    s->destroy = normal_state_destroy;
    s->emission_lprob = normal_state_emission_lprob;
    normal_state_create(s, emission_lprobs);
    return s;
}

NHMM_API const char *nhmm_state_name(const struct nhmm_state *s) { return s->name; }

NHMM_API const struct nhmm_alphabet *nhmm_state_alphabet(const struct nhmm_state *s)
{
    return s->alphabet;
}

NHMM_API void nhmm_state_set_end_state(struct nhmm_state *state, bool end_state)
{
    state->end_state = end_state;
}

NHMM_API double nhmm_state_emission_lprob(struct nhmm_state *state, const char *seq,
                                          size_t seq_len)
{
    return state->emission_lprob(state, seq, seq_len);
}

NHMM_API void nhmm_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    sdsfree(state->name);
    state->alphabet = NULL;
    state->destroy(state->impl);
    free(state);
}

struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *alphabet)
{
    struct nhmm_state *s = malloc(sizeof(struct nhmm_state));
    s->name = sdsnew(name);
    s->alphabet = alphabet;
    return s;
}
