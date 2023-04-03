#include "range.h"
#include "swap.h"
#include <assert.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

struct imm_range imm_range(unsigned a, unsigned b)
{
  assert(a <= b);
  return (struct imm_range){a, b};
}

void imm_range_set(struct imm_range *x, unsigned a, unsigned b)
{
  assert(a <= b);
  x->a = a;
  x->b = b;
}

unsigned imm_range_size(struct imm_range x) { return x.b - x.a; }

bool imm_range_empty(struct imm_range x) { return x.b <= x.a; }

void imm_range_swap(struct imm_range *x, struct imm_range *y)
{
  swap(x->a, y->a);
  swap(x->b, y->b);
}

struct imm_range imm_range_intersect(struct imm_range x, struct imm_range y)
{
  if (x.a <= y.a)
  {
    if (y.a < x.b)
      return imm_range(y.a, MIN(x.b, y.b));
    else
      return imm_range(x.b, x.b);
  }
  return imm_range_intersect(y, x);
}

void imm_range_subtract(struct imm_range x, struct imm_range y,
                        struct imm_range *l, struct imm_range *r)
{
  struct imm_range i = imm_range_intersect(x, y);
  if (imm_range_empty(i))
  {
    imm_range_set(l, x.a, x.b);
    imm_range_set(r, x.b, x.b);
    return;
  }

  imm_range_set(l, x.a, i.a);
  imm_range_set(r, i.b, x.b);
  return;
}

void imm_range_dump(struct imm_range x, FILE *restrict fp)
{
  fprintf(fp, "range[%u, %u)", x.a, x.b);
}
