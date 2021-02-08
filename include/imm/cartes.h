#ifndef IMM_CARTES_H
#define IMM_CARTES_H

#include <stdint.h>

struct imm_cartes;

struct imm_cartes* imm_cartes_create(char const* set, uint16_t set_size, uint16_t max_times);
void               imm_cartes_destroy(struct imm_cartes const* cartes);
char const*        imm_cartes_next(struct imm_cartes* cartes);
void               imm_cartes_setup(struct imm_cartes* cartes, uint16_t times);

#endif
