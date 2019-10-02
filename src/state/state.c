#include "state/state.h"
#include "alphabet.h"
#include "report.h"
#include "state/frame.h"
#include "state/normal.h"
#include "state/silent.h"
#include <stdlib.h>

#define IMM_API_EXPORTS

struct imm_state *state_create(const char *name, const struct imm_alphabet *alphabet);

IMM_API struct imm_state *imm_state_create_normal(const char *name,
                                                  const struct imm_alphabet *alphabet,
                                                  const double *emiss_lprobs)
{
    if (alphabet_length(alphabet) == 0) {
        error("empty alphabet");
        return NULL;
    }
    struct imm_state *s = state_create(name, alphabet);
    s->destroy = normal_state_destroy;
    s->emiss_lprob = normal_state_emiss_lprob;
    s->normalize = normal_state_normalize;
    s->min_seq = normal_state_min_seq;
    s->max_seq = normal_state_max_seq;
    normal_state_create(s, emiss_lprobs);
    return s;
}

IMM_API struct imm_state *imm_state_create_silent(const char *name,
                                                  const struct imm_alphabet *alphabet)
{
    struct imm_state *s = state_create(name, alphabet);
    s->destroy = silent_state_destroy;
    s->emiss_lprob = silent_state_emiss_lprob;
    s->normalize = silent_state_normalize;
    s->min_seq = silent_state_min_seq;
    s->max_seq = silent_state_max_seq;
    silent_state_create(s);
    return s;
}

IMM_API struct imm_state *imm_state_create_frame(const char *name,
                                                 const struct imm_alphabet *alphabet,
                                                 const double *base_lprobs,
                                                 const struct imm_codon *codon,
                                                 double epsilon)
{
    struct imm_state *s = state_create(name, alphabet);
    s->destroy = frame_state_destroy;
    s->emiss_lprob = frame_state_emiss_lprob;
    s->normalize = frame_state_normalize;
    s->min_seq = frame_state_min_seq;
    s->max_seq = frame_state_max_seq;
    frame_state_create(s, base_lprobs, codon, epsilon);
    return s;
}

IMM_API const char *imm_state_get_name(const struct imm_state *s)
{
    return rs_data_c(&s->name);
}

IMM_API const struct imm_alphabet *imm_state_get_alphabet(const struct imm_state *s)
{
    return s->alphabet;
}

IMM_API void imm_state_set_end_state(struct imm_state *state, bool end_state)
{
    state->end_state = end_state;
}

IMM_API double imm_state_emiss_lprob(const struct imm_state *state, const char *seq,
                                     size_t seq_len)
{
    return state->emiss_lprob(state, seq, seq_len);
}

IMM_API int imm_state_normalize(struct imm_state *state)
{
    return state->normalize(state);
}

IMM_API int imm_state_min_seq(const struct imm_state *state)
{
    return state->min_seq(state);
}

IMM_API int imm_state_max_seq(const struct imm_state *state)
{
    return state->max_seq(state);
}

IMM_API void imm_state_destroy(struct imm_state *state)
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

struct imm_state *state_create(const char *name, const struct imm_alphabet *alphabet)
{
    struct imm_state *s = malloc(sizeof(struct imm_state));
    rs_init_w(&s->name, name);
    s->alphabet = alphabet;
    return s;
}
