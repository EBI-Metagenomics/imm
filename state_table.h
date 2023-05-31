#ifndef IMM_STATE_TABLE_H
#define IMM_STATE_TABLE_H

#include "assume.h"
#include "compiler.h"
#include "range.h"
#include "state.h"
#include "zspan.h"
#include <stdint.h>

struct imm_dp_cfg;

struct imm_state_table
{
  unsigned nstates;
  uint16_t *ids;
  struct
  {
    uint16_t state_idx;
    float lprob;
  } start;
  unsigned end_state_idx;
  uint8_t *span;
};

void imm_state_table_init(struct imm_state_table *);
void imm_state_table_cleanup(struct imm_state_table *);
int imm_state_table_reset(struct imm_state_table *, struct imm_dp_cfg const *);
unsigned imm_state_table_idx(struct imm_state_table const *, unsigned id);
unsigned imm_state_table_id(struct imm_state_table const *, unsigned idx);
struct imm_range imm_state_table_range(struct imm_state_table const *,
                                       unsigned idx);
void imm_state_table_dump(struct imm_state_table const *, imm_state_name *,
                          FILE *restrict);

TEMPLATE uint8_t imm_state_table_span(struct imm_state_table const *x,
                                      unsigned const state)
{
  return x->span[state];
}

#endif
