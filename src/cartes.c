#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>

struct imm_cartes
{
    char const* set;
    unsigned    set_size;
    unsigned    times;
    unsigned    iter_idx;
    char*       item;
    unsigned    nitems;
};

struct imm_cartes* imm_cartes_create(char const* set, uint16_t set_size, uint16_t max_times)
{
    struct imm_cartes* cartes = xmalloc(sizeof(*cartes));
    cartes->set = set;
    cartes->set_size = set_size;
    cartes->times = 0;
    cartes->iter_idx = 0;
    cartes->item = malloc(sizeof(*cartes->item) * (unsigned)(max_times + 1));
    if (!cartes->item) {
        error("%s", explain(IMM_OUTOFMEM));
        free(cartes);
        return NULL;
    }
    cartes->nitems = 0;
    return cartes;
}

void imm_cartes_destroy(struct imm_cartes const* cartes)
{
    free((void*)cartes->item);
    free((void*)cartes);
}

char const* imm_cartes_next(struct imm_cartes* cartes)
{
    if (cartes->iter_idx == cartes->nitems)
        return NULL;

    char*    item = cartes->item;
    unsigned idx = cartes->iter_idx++;
    unsigned set_size = cartes->set_size;

    for (unsigned i = 0; i < cartes->times; ++i) {
        item[i] = cartes->set[(idx % ipow(set_size, i + 1)) / ipow(set_size, i)];
    }

    return item;
}

void imm_cartes_setup(struct imm_cartes* cartes, uint16_t times)
{
    cartes->times = times;
    cartes->item[times] = '\0';
    cartes->iter_idx = 0;
    unsigned long nitems = ipow(cartes->set_size, times);
    BUG(nitems > UINT16_MAX);
    cartes->nitems = (uint16_t)nitems;
}
