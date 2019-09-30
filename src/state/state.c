#include "state/state.h"
#include "alphabet.h"
#include "report.h"
#include "state/frame.h"
#include "state/normal.h"
#include "state/silent.h"
#include <stdlib.h>

#define NHMM_API_EXPORTS

struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *alphabet);

NHMM_API struct nhmm_state *nhmm_state_create_normal(
    const char *name, const struct nhmm_alphabet *alphabet, const double *emiss_lprobs)
{
    if (nhmm_alphabet_length(alphabet) == 0) {
        error("empty alphabet");
        return NULL;
    }
    struct nhmm_state *s = state_create(name, alphabet);
    s->destroy = normal_state_destroy;
    s->emiss_lprob = normal_state_emiss_lprob;
    s->normalize = normal_state_normalize;
    normal_state_create(s, emiss_lprobs);
    return s;
}

NHMM_API struct nhmm_state *nhmm_state_create_silent(
    const char *name, const struct nhmm_alphabet *alphabet)
{
    struct nhmm_state *s = state_create(name, alphabet);
    s->destroy = silent_state_destroy;
    s->emiss_lprob = silent_state_emiss_lprob;
    s->normalize = silent_state_normalize;
    silent_state_create(s);
    return s;
}

NHMM_API struct nhmm_state *nhmm_state_create_frame(
    const char *name, const struct nhmm_alphabet *alphabet,
    const double *base_emiss_lprobs, const struct nhmm_codon *codon, double epsilon)
{
    struct nhmm_state *s = state_create(name, alphabet);
    s->destroy = frame_state_destroy;
    s->emiss_lprob = frame_state_emiss_lprob;
    s->normalize = frame_state_normalize;
    frame_state_create(s, base_emiss_lprobs, codon, epsilon);
    return s;
}

NHMM_API const char *nhmm_state_get_name(const struct nhmm_state *s)
{
    return rs_data_c(&s->name);
}

NHMM_API const struct nhmm_alphabet *nhmm_state_get_alphabet(const struct nhmm_state *s)
{
    return s->alphabet;
}

NHMM_API void nhmm_state_set_end_state(struct nhmm_state *state, bool end_state)
{
    state->end_state = end_state;
}

NHMM_API double nhmm_state_emiss_lprob(const struct nhmm_state *state, const char *seq,
                                       size_t seq_len)
{
    return state->emiss_lprob(state, seq, seq_len);
}

NHMM_API int nhmm_state_normalize(struct nhmm_state *state)
{
    return state->normalize(state);
}

NHMM_API void nhmm_state_destroy(struct nhmm_state *state)
{
    if (!state)
        return;

    rs_free(&state->name);
    state->alphabet = NULL;
    state->destroy(state);
    state->destroy = NULL;
    state->emiss_lprob = NULL;
    state->normalize = NULL;
    free(state);
}

struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *alphabet)
{
    struct nhmm_state *s = malloc(sizeof(struct nhmm_state));
    rs_init_w(&s->name, name);
    s->alphabet = alphabet;
    return s;
}
