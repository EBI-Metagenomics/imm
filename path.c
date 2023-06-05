#include "path.h"
#include "printer.h"
#include "rc.h"
#include "reallocf.h"
#include "state.h"
#include "step.h"
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

  size_t count = (unsigned)path->nsteps + 1;
  size_t capacity = (size_t)path->capacity;

  if (sizeof *path->steps * count > capacity)
  {
    capacity <<= 1;
    assert(capacity >= sizeof *path->steps * count);
    path->steps = imm_reallocf(path->steps, capacity);
    if (!path->steps && capacity > 0) return IMM_ENOMEM;
  }

  path->capacity = (int)capacity;
  imm_path_add_unsafe(path, step);
  return rc;
}

struct imm_step *imm_path_step(struct imm_path const *path, unsigned step)
{
  int i = path->start + (int)step * path->dir;
  int n = path->capacity;
  return path->steps + (unsigned)(((i % n) + n) % n);
}

void imm_path_add_unsafe(struct imm_path *path, struct imm_step step)
{
  *imm_path_step(path, (unsigned)path->nsteps++) = step;
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

unsigned imm_path_nsteps(struct imm_path const *path)
{
  return (unsigned)path->nsteps;
}

void imm_path_reverse(struct imm_path *path)
{
  if (path->nsteps == 0) return;
  int i = path->start + (int)path->nsteps * path->dir;
  int n = path->capacity;
  path->start = (((i % n) + n) % n) - 1;
  path->dir *= -1;
}

static char *id_state_name(unsigned id, char *name)
{
  sprintf(name, "%u", id);
  return name;
}

void imm_path_dump(struct imm_path const *x, imm_state_name *callb,
                   struct imm_seq const *seq, FILE *restrict fp)
{
  char name[IMM_STATE_NAME_SIZE] = {0};
  if (!callb) callb = &id_state_name;
  char const *sequence = imm_seq_str(seq);
  for (unsigned i = 0; i < imm_path_nsteps(x); ++i)
  {
    if (i > 0) fprintf(fp, "->");
    unsigned state_id = imm_path_step(x, i)->state_id;
    unsigned seqlen = imm_path_step(x, i)->seqlen;
    float score = imm_path_step(x, i)->seqlen;

    fputc('[', fp);

    fprintf(fp, "%s,", (*callb)(state_id, name));
    fprintf(fp, "%.*s,", seqlen, sequence);
    fprintf(fp, imm_printer_get_f32_formatter(), score);

    fputc(']', fp);

    sequence += seqlen;
  }
  fprintf(fp, "\n");
}
