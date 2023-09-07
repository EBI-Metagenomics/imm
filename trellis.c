#include "trellis.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state_table.h"
#include <stdlib.h>

void imm_trellis_init(struct imm_trellis *x,
                      struct imm_state_table const *state_table)
{
  x->state_table = state_table;
  x->capacity = 0;
  x->num_stages = 0;
  x->num_states = 0;
  x->head = NULL;
  x->pool = NULL;
}

int imm_trellis_setup(struct imm_trellis *x, unsigned seqsize, unsigned nstates)
{
  unsigned num_stages = seqsize + 1;
  size_t size = sizeof(struct imm_node) * (num_stages * nstates);
  if (size > x->capacity)
  {
    if (!(x->pool = imm_reallocf(x->pool, size)))
    {
      imm_trellis_cleanup(x);
      return IMM_ENOMEM;
    }
  }

  x->num_stages = num_stages;
  x->num_states = nstates;
  x->head = x->pool;
  return 0;
}

void imm_trellis_cleanup(struct imm_trellis *x)
{
  if (x)
  {
    x->state_table = NULL;
    x->capacity = 0;
    x->num_stages = 0;
    x->num_states = 0;
    x->head = NULL;
    free(x->pool);
    x->pool = NULL;
  }
}

void imm_trellis_prepare(struct imm_trellis *x)
{
  imm_trellis_rewind(x);
  for (size_t i = 0; i < imm_trellis_size(x); ++i)
  {
    imm_node_invalidate(x->head);
    x->head++;
  }
  imm_trellis_rewind(x);
}

IMM_INLINE unsigned state_id_at(struct imm_trellis const *x,
                                struct imm_node const *head)
{
  return imm_state_table_id(x->state_table, imm_trellis_state_idx_at(x, head));
}

unsigned imm_trellis_state_id(struct imm_trellis const *x)
{
  return state_id_at(x, x->head);
}

void imm_trellis_back(struct imm_trellis *x)
{
  struct imm_node *h = imm_trellis_head(x);
  imm_trellis_seek(x, imm_trellis_stage_idx(x) - h->emission_size,
                   h->state_source);
}

void imm_trellis_dump(struct imm_trellis const *x, FILE *restrict fp)
{
  // HEADER
  for (unsigned i = 0; i < x->num_states; ++i)
  {
    if (i > 0) fputc(',', fp);
    unsigned idx = imm_trellis_state_idx_at(x, imm_trellis_at(x, 0, i));
    fputs(imm_state_table_name(x->state_table, idx), fp);
  }
  fputc('\n', fp);

  // BODY
  for (unsigned i = 0; i < x->num_stages; ++i)
  {
    for (unsigned j = 0; j < x->num_states; ++j)
    {
      if (j > 0) fputc(',', fp);
      imm_node_dump(imm_trellis_at(x, i, j), x->state_table, fp);
    }
    fputc('\n', fp);
  }
}
