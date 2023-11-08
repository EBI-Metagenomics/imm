#ifndef IMM_CARTES_H
#define IMM_CARTES_H

#include "compiler.h"
#include "ipow.h"
#include <assert.h>
#include <stddef.h>

#define IMM_CARTES_SIZE 32

struct imm_cartes
{
  char const *set;
  int set_size;
  int times;
  int iter_idx;
  size_t capacity;
  char item[IMM_CARTES_SIZE];
  int nitems;
};

IMM_INLINE void imm_cartes_init(struct imm_cartes *x, char const *set,
                                int set_size)
{
  x->set = set;
  x->set_size = set_size;
  x->times = 0;
  x->iter_idx = 0;
  x->nitems = 0;
}

IMM_INLINE void imm_cartes_setup(struct imm_cartes *x, int times)
{
  assert(items <= IMM_CARTES_SIZE);
  x->times = times;
  x->iter_idx = 0;
  long nitems = imm_ipow(x->set_size, times);
  assert(nitems <= INT_MAX);
  x->nitems = (int)nitems;
}

IMM_INLINE char const *imm_cartes_next(struct imm_cartes *x)
{
  assert(cartes->nitems > 0);
  if (x->iter_idx == x->nitems) return NULL;

  char *it = x->item;
  int idx = x->iter_idx++;
  int set_size = x->set_size;

  for (int i = 0; i < x->times; ++i)
    it[i] = x->set[(idx % imm_ipow(set_size, i + 1)) / imm_ipow(set_size, i)];

  return it;
}

#endif
