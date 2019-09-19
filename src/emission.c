#include "emission.h"

#include <math.h>
#include <stdlib.h>

struct emission *emission_create(double *lprobs, size_t length)
{
    struct emission *e = malloc(sizeof(struct emission));
    e->lprobs = lprobs;
    e->probs = malloc(sizeof(double) * length);
    for (size_t i = 0; i < length; ++i)
        e->probs[i] = exp(lprobs[i]);
    return e;
}

void emission_destroy(struct emission *emission)
{
    if (!emission)
        return;

    if (emission->lprobs)
        free(emission->lprobs);

    if (emission->probs)
        free(emission->probs);

    free(emission);
}
