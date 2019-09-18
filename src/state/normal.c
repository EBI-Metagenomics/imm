#include "state/normal.h"

#include <stdlib.h>

struct normal_state *create_normal_state(double *emission)
{
    struct normal_state *s = malloc(sizeof(struct nhmm_state));
    s->emission = emission;
    return s;
}

void destroy_normal_state(struct nhmm_state *s)
{
    sdsfree(s->name);
    s->a = NULL;
    if (s->data)
    {
        struct normal_state *ns = s->data;
        if (ns->emission)
            free(ns->emission);
        free(ns);
    }
    free(s);
}
