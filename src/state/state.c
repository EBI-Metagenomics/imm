#define NHMM_API_EXPORTS

#include "state/state.h"
#include "alphabet.h"
#include "state/normal.h"

#include <stdlib.h>

struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *a);

NHMM_API struct nhmm_state *nhmm_state_create_normal(const char *name,
                                                     const struct nhmm_alphabet *a,
                                                     double *emission)
{
    struct nhmm_state *s = state_create(name, a);
    s->destroy = normal_state_destroy;
    s->emission = normal_state_emission;
    normal_state_create(s, emission);
    return s;
}

NHMM_API const char *nhmm_state_name(struct nhmm_state *s) { return s->name; }

NHMM_API double nhmm_state_emission(struct nhmm_state *s, const char *x, size_t xlen)
{
    return s->emission(s, x, xlen);
}

NHMM_API void nhmm_state_destroy(struct nhmm_state *s)
{
    if (!s)
        return;

    sdsfree(s->name);
    s->a = NULL;
    s->destroy(s->impl);
    free(s);
}


struct nhmm_state *state_create(const char *name, const struct nhmm_alphabet *a)
{
    struct nhmm_state *s = malloc(sizeof(struct nhmm_state));
    s->name = sdsnew(name);
    s->a = a;
    return s;
}
