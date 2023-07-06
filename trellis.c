#include "trellis.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state_table.h"
#include "trans_table.h"
#include <stdlib.h>

void imm_trellis_init(struct imm_trellis *x,
                      struct imm_state_table const *state_table,
                      struct imm_trans_table const *trans_table)
{
  x->state_table = state_table;
  x->trans_table = trans_table;
  x->capacity = 0;
  x->sequence_size = 0;
  x->num_states = 0;
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
    x->state_table = NULL;
    x->trans_table = NULL;
    x->capacity = 0;
    x->sequence_size = 0;
    x->num_states = 0;
    x->position = NULL;
    free(x->pool);
    x->pool = NULL;
  }
}

IMM_API unsigned imm_trellis_state_id(struct imm_trellis const *x)
{
  return imm_state_table_id(x->state_table, imm_trellis_state_idx(x));
}

bool imm_trellis_step_back(struct imm_trellis *x)
{
  struct imm_trellis_node *node = x->position;
  unsigned len = node->emission_size;
  unsigned dst = imm_trellis_state_idx(x);
  unsigned src =
      imm_trans_table_source_state(x->trans_table, dst, node->transition);

  node -= len * x->num_states;
  node += src;
  node -= dst;

  if (node < x->pool)
  {
    x->position = x->pool;
    return false;
  }
  x->position = node;
  return true;
}

void imm_trellis_dump(struct imm_trellis const *x, FILE *restrict fp)
{
  struct imm_trellis_node const *node = x->pool;
  for (unsigned i = 0; i < x->sequence_size; ++i)
  {
    for (unsigned j = 0; j < x->num_states; ++j)
    {
      if (j > 0) fputc(',', fp);
      fprintf(fp, "(%u, %u, %.9g)", node->transition, node->emission_size,
              node->score);
      node++;
    }
    fprintf(fp, "\n");
  }
  fprintf(fp, "\n");
}
