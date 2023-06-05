#include "trellis.h"
#include "rc.h"
#include "reallocf.h"
#include "trans_table.h"
#include <stdlib.h>

void imm_trellis_init(struct imm_trellis *x)
{
  x->capacity = 0;
  x->num_states = 0;
  x->sequence_size = 0;
  x->position = NULL;
  x->pool = NULL;
}

int imm_trellis_setup(struct imm_trellis *x, unsigned seqsize, unsigned nstates)
{
  size_t size = sizeof(struct imm_trellis_node) * (seqsize * nstates);
  if (size > x->capacity)
  {
    if (!(x->pool = imm_reallocf(x->pool, size)))
    {
      imm_trellis_cleanup(x);
      return IMM_ENOMEM;
    }
  }

  x->sequence_size = seqsize;
  x->num_states = nstates;
  imm_trellis_rewind(x);
  return 0;
}

void imm_trellis_cleanup(struct imm_trellis *x)
{
  if (x)
  {
    x->capacity = 0;
    x->num_states = 0;
    x->sequence_size = 0;
    x->position = NULL;
    free(x->pool);
    x->pool = NULL;
  }
}

bool imm_trellis_step_back(struct imm_trellis *x,
                           struct imm_trans_table const *tbl)
{
  struct imm_trellis_node const *node = x->position;
  unsigned len = node->emission_length;
  unsigned dst = (node - x->pool) % x->num_states;
  unsigned src = imm_trans_table_source_state(tbl, dst, node->transition);

  node -= len * (sizeof(struct imm_trellis_node) * x->num_states);
  node += src;
  node -= dst;

  if (node < x->pool)
  {
    x->position = x->pool;
    return false;
  }
  return true;
}
