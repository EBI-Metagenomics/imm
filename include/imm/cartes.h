#ifndef IMM_CARTES_H
#define IMM_CARTES_H

#include "imm/export.h"

struct imm_cartes;

IMM_API struct imm_cartes* imm_cartes_create(char const* set, unsigned set_size, unsigned max_times);
IMM_API void               imm_cartes_destroy(struct imm_cartes const* cartes);
IMM_API char const*        imm_cartes_next(struct imm_cartes* cartes);
IMM_API void               imm_cartes_setup(struct imm_cartes* cartes, unsigned times);

#endif
