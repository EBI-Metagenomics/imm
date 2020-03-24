#ifndef IMM_CARTES_H
#define IMM_CARTES_H

struct imm_cartes;

struct imm_cartes* imm_cartes_create(char const* set, unsigned set_size, unsigned max_times);
void               imm_cartes_setup(struct imm_cartes* cartes, unsigned times);
void               imm_cartes_destroy(struct imm_cartes const* cartes);
char const*        imm_cartes_next(struct imm_cartes* cartes);

#endif
