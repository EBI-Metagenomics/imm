#ifndef NHMM_EMISSION_H
#define NHMM_EMISSION_H

#include <stddef.h>

struct emission
{
    double *lprobs;
    double *probs;
    size_t length;
};

struct emission *emission_create(double *lprobs, size_t length);
void emission_destroy(struct emission *emission);

#endif
