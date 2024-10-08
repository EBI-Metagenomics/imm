#include "imm_path.h"
#include "imm_fmt.h"
#include "imm_lprob.h"
#include "imm_rc.h"
#include "imm_reallocf.h"
#include "imm_state.h"
#include "imm_step.h"
#include <assert.h>
#include <stdlib.h>

struct imm_path imm_path(void) { return (struct imm_path){0, 0, 1, 0, NULL}; }

static int path_setup(struct imm_path *path)
{
  path->capacity = sizeof(*path->steps) * (1 << 4);
  path->nsteps = 0;
  path->dir = 1;
  path->start = 0;
  path->steps = malloc((size_t)path->capacity);
  if (!path->steps) return IMM_ENOMEM;
  return 0;
}

int imm_path_add(struct imm_path *path, struct imm_step step)
{
  int rc = 0;
  if (path->capacity == 0)
  {
    if ((rc = path_setup(path))) return rc;
  }

  size_t count = (size_t)(path->nsteps + 1);
  size_t capacity = (size_t)path->capacity;

  if (sizeof(*path->steps) * count > capacity)
  {
    capacity <<= 1;
    assert(capacity >= sizeof *path->steps * count);
    path->steps = imm_reallocf(path->steps, capacity);
    if (!path->steps && capacity > 0) return IMM_ENOMEM;
  }

  path->capacity = (int)capacity;
  assert(path->capacity >= 0);
  imm_path_add_unsafe(path, step);
  return rc;
}

struct imm_step *imm_path_step(struct imm_path const *path, int step)
{
  int i = path->start + step * path->dir;
  int n = path->capacity;
  return path->steps + (((i % n) + n) % n);
}

void imm_path_add_unsafe(struct imm_path *path, struct imm_step step)
{
  *imm_path_step(path, path->nsteps++) = step;
}

void imm_path_cleanup(struct imm_path *x)
{
  if (x)
  {
    free(x->steps);
    x->steps = NULL;
  }
}

void imm_path_reset(struct imm_path *path)
{
  path->nsteps = 0;
  path->dir = 1;
  path->start = 0;
}

int imm_path_nsteps(struct imm_path const *path) { return path->nsteps; }

void imm_path_reverse(struct imm_path *path)
{
  if (path->nsteps == 0) return;
  int i = path->start + path->nsteps * path->dir;
  int n = path->capacity;
  path->start = (((i % n) + n) % n) - 1;
  path->dir *= -1;
}

float imm_path_score(struct imm_path const *x)
{
  if (imm_path_nsteps(x) == 0) return IMM_LPROB_NAN;
  return imm_path_step(x, imm_path_nsteps(x) - 1)->score;
}

void imm_path_cut(struct imm_path *x, int step, int size)
{
  assert(step + size <= x->nsteps);
  x->start = x->start + step * x->dir;
  x->nsteps = size;
}

void imm_path_dump(struct imm_path const *x, imm_state_name *callb,
                   struct imm_seq const *seq, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &imm_state_default_name;
  char const *sequence = imm_seq_data(seq);
  for (int i = 0; i < imm_path_nsteps(x); ++i)
  {
    if (i > 0) fputc(',', fp);
    int state_id = imm_path_step(x, i)->state_id;
    int seqsize = imm_path_step(x, i)->seqsize;
    float score = imm_path_step(x, i)->score;

    fputc('(', fp);

    fprintf(fp, "%s;", (*callb)(state_id, name));
    fprintf(fp, "%.*s;", seqsize, sequence);
    fprintf(fp, imm_fmt_get_f32(), score);

    fputc(')', fp);

    sequence += seqsize;
  }
  fputc('\n', fp);
}
