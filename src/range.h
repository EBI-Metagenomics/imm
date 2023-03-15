#ifndef RANGE_H
#define RANGE_H

#include <stdbool.h>

// [a, b)
struct range
{
    unsigned a;
    unsigned b;
};

struct range range_init(unsigned a, unsigned b);
void range_set(struct range *x, unsigned a, unsigned b);
unsigned range_size(struct range x);
bool range_empty(struct range x);
struct range range_intersect(struct range x, struct range y);
void range_subtract(struct range x, struct range y, struct range *l,
                    struct range *r);

#endif
