#include "hot_range.h"
#include "viterbi.h"
#include "zspan.h"
#include <stdio.h>
#include <stdlib.h>

IMM_TEMPLATE unsigned source(struct imm_viterbi const *vit, unsigned state,
                             unsigned t)
{
  return imm_viterbi_source(vit, state, t);
}

IMM_TEMPLATE uint8_t span(struct imm_viterbi const *x, unsigned const state)
{
  return imm_viterbi_span(x, state);
}

struct imm_hot_range imm_hot_range(struct imm_viterbi const *vit)
{
  struct imm_hot_range x = {0};

  unsigned start = 0;
  unsigned nstates = imm_viterbi_nstates(vit);
  while (start < nstates)
  {
    if (imm_viterbi_ntrans(vit, start) != 2)
    {
      start += 1;
      continue;
    }

    uint8_t dst_span = span(vit, start);
    unsigned left[2] = {source(vit, start, 0), source(vit, start, 1)};
    uint8_t src_spans[2] = {span(vit, source(vit, start, 0)),
                            span(vit, source(vit, start, 1))};

    unsigned end = start + 1;

    if (end >= nstates) break;
    if (imm_viterbi_ntrans(vit, end) != 2 || dst_span != span(vit, end) ||
        src_spans[0] != span(vit, source(vit, end, 0)) ||
        src_spans[1] != span(vit, source(vit, end, 1)))
    {
      start = end;
      continue;
    }

    unsigned right[2] = {source(vit, end, 0), source(vit, end, 1)};

    int pattern[2] = {right[0] - left[0], right[1] - left[1]};

    while (++end < nstates)
    {
      if (imm_viterbi_ntrans(vit, end) != 2) break;
      if (src_spans[0] != span(vit, source(vit, end, 0))) break;
      if (src_spans[1] != span(vit, source(vit, end, 1))) break;
      left[0] = right[0];
      left[1] = right[1];
      right[0] = source(vit, end, 0);
      right[1] = source(vit, end, 1);
      int t[2] = {right[0] - left[0], right[1] - left[1]};
      if (t[0] != pattern[0] || t[1] != pattern[1]) break;
    }

    if (end - start > x.end - x.start)
    {
      x.start = start;
      x.end = end;
      x.span = dst_span;
      x.pattern[0] = pattern[0];
      x.pattern[1] = pattern[1];
      x.src_spans[0] = src_spans[0];
      x.src_spans[1] = src_spans[1];
    }
    start = end;
  }
  return x;
}

int imm_hot_range_code(struct imm_hot_range const *x)
{
  char code[] = {
      imm_zspan_min(x->src_spans[0]) + '0',
      imm_zspan_max(x->src_spans[0]) + '0',
      imm_zspan_min(x->src_spans[1]) + '0',
      imm_zspan_max(x->src_spans[1]) + '0',
      imm_zspan_min(x->span) + '0',
      imm_zspan_max(x->span) + '0',
      '\0',
  };
  return atoi(code);
}

void imm_hot_range_dump(struct imm_hot_range const *x, FILE *fp)
{
  fprintf(fp, "hot_range[%u, %u): ", x->start, x->end);
  fprintf(fp, "span[%u, %u], ", imm_zspan_min(x->span), imm_zspan_max(x->span));
  fprintf(fp, "pattern(%d, %d), ", x->pattern[0], x->pattern[1]);
  fprintf(fp, "src[0]_span[%d, %d], ", imm_zspan_min(x->src_spans[0]),
          imm_zspan_max(x->src_spans[0]));
  fprintf(fp, "src[1]_span[%d, %d]\n", imm_zspan_min(x->src_spans[1]),
          imm_zspan_max(x->src_spans[1]));
}
