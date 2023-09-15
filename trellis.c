#include "trellis.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "state_table.h"
#include <stdlib.h>

void imm_trellis_init(struct imm_trellis *x)
{
  x->ids = NULL;
  x->state_name = &imm_state_default_name;
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
    x->ids = NULL;
    x->state_name = NULL;
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

void imm_trellis_set_ids(struct imm_trellis *x, uint16_t *ids) { x->ids = ids; }

void imm_trellis_set_state_name(struct imm_trellis *x, imm_state_name *callb)
{
  x->state_name = callb;
}

void imm_trellis_back(struct imm_trellis *x)
{
  struct imm_node *h = imm_trellis_head(x);
  imm_trellis_seek(x, imm_trellis_stage_idx(x) - h->emission_size,
                   h->state_source);
}

void imm_trellis_dump(struct imm_trellis const *x, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  assert(x->ids);
  assert(x->state_name);
  // HEADER
  for (unsigned i = 0; i < x->num_states; ++i)
  {
    if (i > 0) fputc(',', fp);
    unsigned idx = imm_trellis_state_idx_at(x, imm_trellis_at(x, 0, i));
    if (idx == IMM_STATE_NULL_IDX) fputs("?", fp);
    else fputs((*x->state_name)(x->ids[idx], name), fp);
  }
  fputc('\n', fp);

  // BODY
  for (unsigned i = 0; i < x->num_stages; ++i)
  {
    for (unsigned j = 0; j < x->num_states; ++j)
    {
      if (j > 0) fputc(',', fp);
      imm_node_dump(imm_trellis_at(x, i, j), x->ids, x->state_name, fp);
    }
    fputc('\n', fp);
  }
}
