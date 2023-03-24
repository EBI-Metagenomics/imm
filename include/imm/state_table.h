#ifndef IMM_STATE_TABLE_H
#define IMM_STATE_TABLE_H

#include "imm/range.h"
#include "imm/state_types.h"

struct span;
struct imm_dp_args;

struct imm_state_table
{
    unsigned nstates;
    imm_state_id_t *ids;
    struct
    {
        imm_state_idx_t state;
        imm_float lprob;
    } start;
    unsigned end_state_idx;
    struct span *span;
};

void imm_state_table_init(struct imm_state_table *);
void imm_state_table_del(struct imm_state_table const *);
int imm_state_table_reset(struct imm_state_table *, struct imm_dp_args const *);
unsigned imm_state_table_idx(struct imm_state_table const *, unsigned id);
unsigned imm_state_table_id(struct imm_state_table const *, unsigned idx);
struct span imm_state_table_span(struct imm_state_table const *, unsigned idx);
struct imm_range imm_state_table_range(struct imm_state_table const *,
                                       unsigned idx);
void imm_state_table_dump(struct imm_state_table const *);

#endif
