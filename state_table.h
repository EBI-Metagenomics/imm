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
  uint16_t start_state_idx;
  unsigned end_state_idx;
  uint8_t *span;

  // Debugging purpose
  imm_state_name *state_name_callback;
};

void imm_state_table_init(struct imm_state_table *);
void imm_state_table_set_name(struct imm_state_table *, imm_state_name *);
void imm_state_table_cleanup(struct imm_state_table *);
int imm_state_table_reset(struct imm_state_table *, struct imm_dp_cfg const *);
unsigned imm_state_table_idx(struct imm_state_table const *, unsigned id);
char *imm_state_table_name(struct imm_state_table const *, unsigned idx);
unsigned imm_state_table_id(struct imm_state_table const *, unsigned idx);
struct imm_range imm_state_table_range(struct imm_state_table const *,
                                       unsigned idx);
void imm_state_table_dump(struct imm_state_table const *, FILE *restrict);

imm_pure_template uint8_t imm_state_table_zspan(struct imm_state_table const *x,
                                                unsigned state_idx)
{
  return x->span[state_idx];
}

#endif
