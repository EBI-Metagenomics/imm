#ifndef IMM_TABLE_STATE_H
#define IMM_TABLE_STATE_H

#include "compiler.h"
#include "span.h"
#include "state.h"

struct imm_abc;

typedef float(imm_table_state_callb)(unsigned size, char const *seq);

struct imm_table_state
{
  struct imm_state super;
  imm_table_state_callb *callback;
};

IMM_API void imm_table_state_init(struct imm_table_state *, unsigned id,
                                  struct imm_abc const *,
                                  imm_table_state_callb *, struct imm_span);

#endif
