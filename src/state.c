#define NHMM_API_EXPORTS

#include "state.h"
#include "alphabet.h"
#include <stdlib.h>

void destroy_normal_state(struct nhmm_state *s);

NHMM_API struct nhmm_state *nhmm_state_create_normal(const char *name,
                                                     const struct nhmm_alphabet *a)
{
    struct nhmm_state *s = malloc(sizeof(struct nhmm_state));
    s->name = sdsnew(name);
    s->a = a;
    s->destroy = destroy_normal_state;
    return s;
}

NHMM_API const char *nhmm_state_name(struct nhmm_state *s) { return s->name; }

NHMM_API void nhmm_state_destroy(struct nhmm_state *s)
{
    if (!s)
        return s->destroy(s);
}

void destroy_normal_state(struct nhmm_state *s)
{
    sdsfree(s->name);
    s->a = NULL;
    free(s);
}
