#include "cartes.h"
#include "free.h"
#include "imm/bug.h"
#include "ipow.h"
#include <limits.h>
#include <stdio.h>
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

struct imm_cartes* imm_cartes_create(char const* set, unsigned set_size, unsigned max_times)
{
    struct imm_cartes* cartes = malloc(sizeof(struct imm_cartes));

    cartes->set = set;
    cartes->set_size = set_size;
    cartes->times = 0;
    cartes->iter_idx = 0;
    cartes->item = malloc(sizeof(char) * (max_times + 1));
    cartes->nitems = 0;

    return cartes;
}

void imm_cartes_setup(struct imm_cartes* cartes, unsigned times)
{
    cartes->times = times;
    cartes->item[times] = '\0';
    cartes->iter_idx = 0;
    unsigned long nitems = imm_ipow(cartes->set_size, times);
    IMM_BUG(nitems > UINT_MAX);
    cartes->nitems = (unsigned)nitems;
}

void imm_cartes_destroy(struct imm_cartes const* cartes)
{
    imm_free(cartes->item);
    imm_free(cartes);
}

char const* imm_cartes_next(struct imm_cartes* cartes)
{
    if (cartes->iter_idx == cartes->nitems)
        return NULL;

    char*    item = cartes->item;
    unsigned idx = cartes->iter_idx++;
    unsigned set_size = cartes->set_size;

    for (unsigned i = 0; i < cartes->times; ++i) {
        item[i] = cartes->set[(idx % imm_ipow(set_size, i + 1)) / imm_ipow(set_size, i)];
    }

    return item;
}
