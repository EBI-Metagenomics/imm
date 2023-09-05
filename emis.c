#include "emis.h"
#include "abc.h"
#include "cartes.h"
#include "code.h"
#include "dump.h"
#include "lprob.h"
#include "rc.h"
#include "reallocf.h"
#include "score_table.h"
#include "state.h"
#include "state_table.h"
#include <assert.h>
#include <stdlib.h>

static inline void cleanup(struct imm_emis *x)
{
  if (x->score)
  {
    free(x->score);
    x->score = NULL;
  }
  if (x->offset)
  {
    free(x->offset);
    x->offset = NULL;
  }
}

void imm_emis_init(struct imm_emis *emis)
{
  emis->score = NULL;
  emis->offset = NULL;
}

void imm_emis_cleanup(struct imm_emis *x)
{
  if (x)
  {
    free(x->score);
    free(x->offset);
    x->score = NULL;
    x->offset = NULL;
  }
}

static int realloc_offset(struct imm_emis *emis, unsigned nstates)
{
  unsigned offsize = imm_emis_offset_size(nstates);
  emis->offset = imm_reallocf(emis->offset, sizeof(*emis->offset) * offsize);
  if (!emis->offset) return IMM_ENOMEM;
  return 0;
}

static int realloc_score(struct imm_emis *emis, unsigned nstates)
{
  unsigned score_size = imm_emis_score_size(emis, nstates);
  emis->score = imm_reallocf(emis->score, sizeof(*emis->score) * score_size);
  if (!emis->score && score_size > 0) return IMM_ENOMEM;
  return 0;
}

static void calc_offset(struct imm_emis *emis, struct imm_code const *code,
                        unsigned nstates, struct imm_state **states)
{
  emis->offset[0] = 0;
  unsigned size = imm_code_size(code, imm_state_span(states[0]));
  for (unsigned i = 1; i < nstates; ++i)
  {
    assert(size <= UINT32_MAX);
    emis->offset[i] = (uint32_t)size;
    size += imm_code_size(code, imm_state_span(states[i]));
  }
  assert(size <= UINT32_MAX);
  emis->offset[nstates] = (uint32_t)size;
}

static void calc_score(struct imm_emis *emis, struct imm_code const *code,
                       unsigned nstates, struct imm_state **states)
{
  struct imm_score_table score_table = {0};
  imm_score_table_init(&score_table, code);

  for (unsigned i = 0; i < nstates; ++i)
  {
    float *scores = emis->score + emis->offset[i];
    imm_score_table_scores(&score_table, states[i], scores);
  }

  imm_score_table_cleanup(&score_table);
}

int imm_emis_reset(struct imm_emis *emis, struct imm_code const *code,
                   struct imm_state **states, unsigned nstates)
{
  int rc = 0;
  if ((rc = realloc_offset(emis, nstates))) goto cleanup;
  calc_offset(emis, code, nstates, states);

  if ((rc = realloc_score(emis, nstates))) goto cleanup;
  calc_score(emis, code, nstates, states);

  return rc;

cleanup:
  cleanup(emis);
  return rc;
}

unsigned imm_emis_score_size(struct imm_emis const *x, unsigned nstates)
{
  return x->offset[nstates];
}

unsigned imm_emis_offset_size(unsigned nstates) { return nstates + 1; }

void imm_emis_dump(struct imm_emis const *x, struct imm_state_table const *st,
                   imm_state_name *callb, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &imm_state_default_name;

  for (unsigned i = 0; i < st->nstates; ++i)
  {
    unsigned size = 0;
    (*callb)(imm_state_table_id(st, i), name);
    float const *score = imm_emis_table(x, i, &size);
    fprintf(fp, "%s=", name);
    imm_dump_array_f32(size, score, fp);
    fputc('\n', fp);
  }
}
