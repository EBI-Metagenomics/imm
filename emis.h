#ifndef IMM_EMIS_H
#define IMM_EMIS_H

#include "api.h"
#include "compiler.h"
#include "state.h"
#include <stdint.h>
#include <stdio.h>

struct imm_emis
{
  float *score;     /**< Sequence emission score of a state. */
  uint32_t *offset; /**< Maps state to score array offset. */
};

struct imm_cartes;
struct imm_code;
struct imm_state;
struct imm_state_table;

void imm_emis_init(struct imm_emis *);
void imm_emis_cleanup(struct imm_emis *);
int imm_emis_reset(struct imm_emis *, struct imm_code const *,
                   struct imm_state **states, unsigned nstates);
unsigned imm_emis_score_size(struct imm_emis const *, unsigned nstates);
unsigned imm_emis_offset_size(unsigned nstates);

imm_pure_template float imm_emis_score(struct imm_emis const *x, unsigned state,
                                       unsigned seq_code)
{
  return x->score[x->offset[state] + seq_code];
}

imm_template float const *imm_emis_table(struct imm_emis const *x,
                                         unsigned state, unsigned *size)
{
  *size = x->offset[state + 1] - x->offset[state];
  return &x->score[x->offset[state]];
}

IMM_API void imm_emis_dump(struct imm_emis const *,
                           struct imm_state_table const *, imm_state_name *,
                           FILE *restrict);

#endif
