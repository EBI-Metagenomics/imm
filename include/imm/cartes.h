#ifndef IMM_CARTES_H
#define IMM_CARTES_H

#include "imm/export.h"
#include <stdint.h>

struct imm_cartes;

IMM_API struct imm_cartes* imm_cartes_create(char const* set, uint16_t set_size, uint16_t max_times);
IMM_API void               imm_cartes_destroy(struct imm_cartes const* cartes);
IMM_API char const*        imm_cartes_next(struct imm_cartes* cartes);
IMM_API void               imm_cartes_setup(struct imm_cartes* cartes, uint16_t times);

#endif
