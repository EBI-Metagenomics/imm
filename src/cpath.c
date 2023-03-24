#include "cpath.h"
#include "bits.h"
#include "error.h"
#include "imm/state_table.h"
#include "imm/trans_table.h"
#include "minmax.h"
#include "reallocf.h"
#include "span.h"
#include <stdlib.h>

int imm_cpath_init(struct cpath *path, struct imm_state_table const *state_tbl,
                   struct imm_trans_table const *trans_tbl)
{
    path->state_offset = NULL;
    path->trans_bits = NULL;
    int rc = imm__cpath_reset(path, state_tbl, trans_tbl);
    if (rc) return rc;
    path->bit = NULL;
    return IMM_OK;
}

int imm__cpath_reset(struct cpath *p, struct imm_state_table const *state_tbl,
                     struct imm_trans_table const *trans_tbl)
{
    unsigned n = p->nstates = state_tbl->nstates;

    p->state_offset =
        imm_reallocf(p->state_offset, sizeof *p->state_offset * (n + 1));

    if (!p->state_offset) return error(IMM_NOMEM);
    p->state_offset[0] = 0;

    p->trans_bits = imm_reallocf(p->trans_bits, sizeof *p->trans_bits * n);
    if (n > 0 && !p->trans_bits) return error(IMM_NOMEM);

    for (unsigned dst = 0; dst < n; ++dst)
    {
        unsigned depth = 0;
        for (unsigned i = 0; i < imm_trans_table_ntrans(trans_tbl, dst); ++i)
        {

            unsigned src = imm_trans_table_source_state(trans_tbl, dst, i);
            unsigned min_seq = imm_state_table_span(state_tbl, src).min;
            unsigned max_seq = imm_state_table_span(state_tbl, src).max;
            depth = MAX(max_seq - min_seq, depth);
        }
        p->state_offset[dst + 1] = p->state_offset[dst];
        p->trans_bits[dst] = (uint8_t)bits_width(
            (uint32_t)imm_trans_table_ntrans(trans_tbl, dst));
        p->state_offset[dst + 1] =
            (uint16_t)(p->state_offset[dst + 1] + p->trans_bits[dst]);
        /* Additional bit (if necessary) for invalid transition or seq_len */
        p->state_offset[dst + 1] =
            (uint16_t)(p->state_offset[dst + 1] +
                       bits_width((uint32_t)((unsigned)depth + 1)));
    }
    return IMM_OK;
}

void imm__cpath_del(struct cpath const *path)
{
    free(path->state_offset);
    free(path->trans_bits);
    if (path->bit) free(path->bit);
}

int imm_cpath_setup(struct cpath *path, unsigned len)
{
    size_t size = path->state_offset[path->nstates] * len;
    path->bit = imm_bitmap_reallocf(path->bit, size);
    if (!path->bit && size > 0) return error(IMM_NOMEM);
    return IMM_OK;
}
