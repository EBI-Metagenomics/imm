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
  x->head = NULL;
  x->pool = NULL;
}

int imm_trellis_setup(struct imm_trellis *x, unsigned seqsize, unsigned nstates)
{
  size_t size = sizeof(struct imm_node) * (seqsize * nstates);
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
  x->head = x->pool;
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
    x->head = NULL;
    free(x->pool);
    x->pool = NULL;
  }
}

IMM_INLINE unsigned state_id_at(struct imm_trellis const *x,
                                struct imm_node const *node)
{
  return imm_state_table_id(x->state_table, imm_trellis_state_idx_at(x, node));
}

unsigned imm_trellis_state_id(struct imm_trellis const *x)
{
  return state_id_at(x, x->head);
}

bool imm_trellis_back(struct imm_trellis *x)
{
  if (!imm_trellis_has_back(x)) return false;

  struct imm_node *h = imm_trellis_head(x);
  imm_trellis_seek(x, imm_trellis_sequence_idx(x) - h->emission_size,
                   h->state_source);

  return true;
}

static char *id_state_name(unsigned id, char *name)
{
  sprintf(name, "%u", id);
  return name;
}

void imm_trellis_dump(struct imm_trellis const *x, imm_state_name *callb,
                      FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &id_state_name;

  // HEADER
  for (unsigned i = 0; i < x->num_states; ++i)
  {
    if (i > 0) fputc(',', fp);
    fputs((*callb)(state_id_at(x, imm_trellis_at(x, 0, i)), name), fp);
  }
  fputc('\n', fp);

  // BODY
  for (unsigned i = 0; i < x->sequence_size; ++i)
  {
    for (unsigned j = 0; j < x->num_states; ++j)
    {
      if (j > 0) fputc(',', fp);
      imm_node_dump(imm_trellis_at(x, i, j), x->state_table, callb, fp);
    }
    fputc('\n', fp);
  }
  fputc('\n', fp);
}
