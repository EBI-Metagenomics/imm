#include "cpath.h"
#include "bits.h"
#include "max.h"
#include "rc.h"
#include "reallocf.h"
#include "span.h"
#include "state_table.h"
#include "trans_table.h"
#include "zspan.h"
#include <stdlib.h>

int imm_cpath_init(struct imm_cpath *x, struct imm_state_table const *state_tbl,
                   struct imm_trans_table const *trans_tbl)
{
  x->nstates = 0;
  x->ncols = 0;
  x->state_offset = NULL;
  x->trans_bits = NULL;
  int rc = imm_cpath_reset(x, state_tbl, trans_tbl);
  if (rc) return rc;
  x->bit = NULL;
  return 0;
}

int imm_cpath_reset(struct imm_cpath *x,
                    struct imm_state_table const *state_tbl,
                    struct imm_trans_table const *trans_tbl)
{
  unsigned n = x->nstates = state_tbl->nstates;

  x->state_offset =
      imm_reallocf(x->state_offset, sizeof *x->state_offset * (n + 1));

  if (!x->state_offset) return IMM_ENOMEM;
  x->state_offset[0] = 0;

  x->trans_bits = imm_reallocf(x->trans_bits, sizeof *x->trans_bits * n);
  if (n > 0 && !x->trans_bits) return IMM_ENOMEM;

  for (unsigned dst = 0; dst < n; ++dst)
  {
    unsigned depth = 0;
    for (unsigned i = 0; i < imm_trans_table_ntrans(trans_tbl, dst); ++i)
    {

      unsigned src = imm_trans_table_source_state(trans_tbl, dst, i);
      unsigned min_seq = imm_zspan_min(imm_state_table_span(state_tbl, src));
      unsigned max_seq = imm_zspan_max(imm_state_table_span(state_tbl, src));
      depth = imm_max(max_seq - min_seq, depth);
    }
    x->state_offset[dst + 1] = x->state_offset[dst];
    x->trans_bits[dst] =
        (uint8_t)bits_width((uint32_t)imm_trans_table_ntrans(trans_tbl, dst));
    x->state_offset[dst + 1] =
        (uint16_t)(x->state_offset[dst + 1] + x->trans_bits[dst]);
    /* Additional bit (if necessary) for invalid transition or seq_len */
    x->state_offset[dst + 1] =
        (uint16_t)(x->state_offset[dst + 1] +
                   bits_width((uint32_t)((unsigned)depth + 1)));
  }
  x->ncols = x->state_offset[x->nstates];
  return 0;
}

void imm_cpath_cleanup(struct imm_cpath *x)
{
  if (x)
  {
    free(x->state_offset);
    free(x->trans_bits);
    free(x->bit);
    x->state_offset = NULL;
    x->trans_bits = NULL;
    x->bit = NULL;
  }
}

int imm_cpath_setup(struct imm_cpath *x, unsigned len)
{
  size_t size = x->state_offset[x->nstates] * len;
  x->bit = imm_bitmap_reallocf(x->bit, size);
  if (!x->bit && size > 0) return IMM_ENOMEM;
  return 0;
}
