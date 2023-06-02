#include "range.h"
#include "assume.h"
#include "min.h"
#include "swap.h"
#include <assert.h>

struct imm_range imm_range(unsigned start, unsigned stop)
{
  assert(start <= stop);
  imm_assume(start <= stop);
  return (struct imm_range){start, stop};
}

void imm_range_set(struct imm_range *x, unsigned start, unsigned stop)
{
  assert(start <= stop);
  imm_assume(start <= stop);
  x->start = start;
  x->stop = stop;
}

unsigned imm_range_size(struct imm_range x) { return x.stop - x.start; }

bool imm_range_empty(struct imm_range x) { return x.stop <= x.start; }

void imm_range_swap(struct imm_range *x, struct imm_range *y)
{
  imm_swap(x->start, y->start);
  imm_swap(x->stop, y->stop);
}

struct imm_range imm_range_intersect(struct imm_range x, struct imm_range y)
{
  if (x.start <= y.start)
  {
    if (y.start < x.stop) return imm_range(y.start, imm_min(x.stop, y.stop));
    else return imm_range(x.stop, x.stop);
  }
  return imm_range_intersect(y, x);
}

void imm_range_subtract(struct imm_range x, struct imm_range y,
                        struct imm_range *l, struct imm_range *r)
{
  struct imm_range i = imm_range_intersect(x, y);
  if (imm_range_empty(i))
  {
    imm_range_set(l, x.start, x.stop);
    imm_range_set(r, x.stop, x.stop);
    return;
  }

  imm_range_set(l, x.start, i.start);
  imm_range_set(r, i.stop, x.stop);
  return;
}

void imm_range_dump(struct imm_range x, FILE *restrict fp)
{
  fprintf(fp, "range[%u, %u)", x.start, x.stop);
}
