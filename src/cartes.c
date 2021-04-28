#include "cartes.h"
#include "common/common.h"
#include <limits.h>

/* TODO: create imm_cartes_reset */

struct cartes *cartes_new(char const *set, unsigned set_size,
                          unsigned max_times)
{
    struct cartes *cartes = xmalloc(sizeof(*cartes));
    cartes->set = set;
    cartes->set_size = set_size;
    cartes->times = 0;
    cartes->iter_idx = 0;
    cartes->item = xmalloc(sizeof(*cartes->item) * (unsigned)(max_times + 1));
    cartes->nitems = 0;
    return cartes;
}

void cartes_del(struct cartes const *cartes)
{
    free((void *)cartes->item);
    free((void *)cartes);
}

char const *cartes_next(struct cartes *cartes)
{
    if (cartes->iter_idx == cartes->nitems)
        return NULL;

    char *item = cartes->item;
    unsigned idx = cartes->iter_idx++;
    unsigned set_size = cartes->set_size;

    for (unsigned i = 0; i < cartes->times; ++i)
    {
        item[i] =
            cartes->set[(idx % ipow(set_size, i + 1)) / ipow(set_size, i)];
    }

    return item;
}

void cartes_setup(struct cartes *cartes, unsigned times)
{
    cartes->times = times;
    cartes->item[times] = '\0';
    cartes->iter_idx = 0;
    unsigned long nitems = ipow(cartes->set_size, times);
    BUG(nitems > UINT_MAX);
    cartes->nitems = (unsigned)nitems;
}
