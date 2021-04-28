#ifndef CARTES_H
#define CARTES_H

struct cartes
{
    char const *set;
    unsigned set_size;
    unsigned times;
    unsigned iter_idx;
    char *item;
    unsigned nitems;
};

struct cartes *cartes_new(char const *set, unsigned set_size,
                          unsigned max_times);

void cartes_del(struct cartes const *cartes);

char const *cartes_next(struct cartes *cartes);

void cartes_setup(struct cartes *cartes, unsigned times);

#endif
