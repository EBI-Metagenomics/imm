#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "state/normal.h"
#include "state/state.h"

#include <stdlib.h>

NHMM_API struct nhmm_state *nhmm_state_create_normal(const char *name,
                                                     const struct nhmm_alphabet *a,
                                                     double *emission)
{
    struct nhmm_state *s = malloc(sizeof(struct nhmm_state));
    s->name = sdsnew(name);
    s->a = a;
    s->destroy = destroy_normal_state;
    s->data = create_normal_state(emission);
    return s;
}

NHMM_API const char *nhmm_state_name(struct nhmm_state *s) { return s->name; }

NHMM_API const char *nhmm_state_emission(struct nhmm_state *s, const char *x,
                                         size_t xlen)
{
    /* if (xlen != 1) */
    /*     return -NHMM_INF; */

    /* struct normal_state *ns = s->data; */
    /* return ns->emission[s->a->char2int[x[0]]]; */
}

NHMM_API void nhmm_state_destroy(struct nhmm_state *s)
{
    if (!s)
        return s->destroy(s);
}
