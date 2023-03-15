#include "range.h"
#include <assert.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

struct range range_init(unsigned a, unsigned b)
{
    assert(a <= b);
    return (struct range){a, b};
}

void range_set(struct range *x, unsigned a, unsigned b)
{
    assert(a <= b);
    x->a = a;
    x->b = b;
}

unsigned range_size(struct range x) { return x.b - x.a; }

bool range_empty(struct range x) { return x.b <= x.a; }

struct range range_intersect(struct range x, struct range y)
{
    if (x.a <= y.a)
    {
        if (y.a < x.b)
            return range_init(y.a, MIN(x.b, y.b));
        else
            return range_init(x.a, x.a);
    }
    return range_intersect(y, x);
}

void range_subtract(struct range x, struct range y, struct range *l,
                    struct range *r)
{
    struct range i = range_intersect(x, y);
    if (range_empty(i))
    {
        range_set(l, x.a, x.b);
        range_set(r, x.b, x.b);
        return;
    }

    range_set(l, x.a, i.a);
    range_set(r, i.b, x.b);
    return;
}
