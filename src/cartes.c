#include "imm/cartes.h"
#include "error.h"
#include "imm/imm.h"
#include "ipow.h"
#include <assert.h>
#include <limits.h>
#include <stdlib.h>

void imm_cartes_init(struct imm_cartes *cartes, char const *set,
                     unsigned set_size, unsigned max_times)
{
    cartes->capacity = 0;
    cartes->item = NULL;
    imm_cartes_reset(cartes, set, set_size, max_times);
}

void imm_cartes_deinit(struct imm_cartes const *cartes)
{
    free((void *)cartes->item);
}

char const *imm_cartes_next(struct imm_cartes *cartes)
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

enum imm_rc imm_cartes_reset(struct imm_cartes *cartes, char const *set,
                             unsigned set_size, unsigned max_times)
{
    cartes->set = set;
    cartes->set_size = set_size;
    cartes->times = 0;
    cartes->iter_idx = 0;
    size_t new_capacity = sizeof(*cartes->item) * (unsigned)(max_times + 1);
    if (new_capacity > cartes->capacity)
    {
        cartes->item = realloc(cartes->item, new_capacity);
        if (new_capacity > 0 && !cartes->item)
            return error(IMM_OUTOFMEM, "failed to realloc");
        cartes->capacity = new_capacity;
    }
    cartes->nitems = 0;
    return IMM_SUCCESS;
}

void imm_cartes_setup(struct imm_cartes *cartes, unsigned times)
{
    cartes->times = times;
    cartes->item[times] = '\0';
    cartes->iter_idx = 0;
    unsigned long nitems = ipow(cartes->set_size, times);
    assert(nitems <= UINT_MAX);
    cartes->nitems = (unsigned)nitems;
}
