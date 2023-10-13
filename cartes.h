#ifndef IMM_CARTES_H
#define IMM_CARTES_H

#include "compiler.h"
#include <stddef.h>

struct imm_cartes
{
  char const *set;
  int set_size;
  int times;
  int iter_idx;
  size_t capacity;
  char *item;
  int nitems;
};

IMM_API void imm_cartes_init(struct imm_cartes *, char const *set,
                             int set_size, int max_times);
IMM_API void imm_cartes_cleanup(struct imm_cartes *);
IMM_API char const *imm_cartes_next(struct imm_cartes *);
IMM_API int imm_cartes_reset(struct imm_cartes *, char const *set,
                             int set_size, int max_times);
IMM_API void imm_cartes_setup(struct imm_cartes *, int times);

#endif
