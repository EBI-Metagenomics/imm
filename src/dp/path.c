#include "dp/path.h"
#include "bits.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "error.h"
#include "minmax.h"
#include "reallocf.h"
#include <stdlib.h>

enum imm_rc imm_path_init(struct path *path,
                          struct imm_dp_state_table const *state_tbl,
                          struct imm_dp_trans_table const *trans_tbl)
{
    path->state_offset = NULL;
    path->trans_bits = NULL;
    enum imm_rc rc = imm__path_reset(path, state_tbl, trans_tbl);
    if (rc) return rc;
    path->bit = NULL;
    return IMM_OK;
}

enum imm_rc imm__path_reset(struct path *p,
                            struct imm_dp_state_table const *state_tbl,
                            struct imm_dp_trans_table const *trans_tbl)
{
    unsigned n = p->nstates = state_tbl->nstates;

    p->state_offset =
        reallocf(p->state_offset, sizeof *p->state_offset * (n + 1));

    if (!p->state_offset) return error(IMM_NOMEM);
    p->state_offset[0] = 0;

    p->trans_bits = reallocf(p->trans_bits, sizeof *p->trans_bits * n);
    if (n > 0 && !p->trans_bits) return error(IMM_NOMEM);

    for (unsigned dst = 0; dst < n; ++dst)
    {
        unsigned depth = 0;
        for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst); ++i)
        {

            unsigned src = trans_table_source_state(trans_tbl, dst, i);
            unsigned min_seq = state_table_span(state_tbl, src).min;
            unsigned max_seq = state_table_span(state_tbl, src).max;
            depth = max(max_seq - min_seq, depth);
        }
        p->state_offset[dst + 1] = p->state_offset[dst];
        p->trans_bits[dst] =
            (uint8_t)bits_width((uint32_t)trans_table_ntrans(trans_tbl, dst));
        p->state_offset[dst + 1] =
            (uint16_t)(p->state_offset[dst + 1] + p->trans_bits[dst]);
        /* Additional bit (if necessary) for invalid transition or seq_len */
        p->state_offset[dst + 1] =
            (uint16_t)(p->state_offset[dst + 1] +
                       bits_width((uint32_t)((unsigned)depth + 1)));
    }
    return IMM_OK;
}

void imm__path_del(struct path const *path)
{
    free(path->state_offset);
    free(path->trans_bits);
    if (path->bit) free(path->bit);
}

enum imm_rc imm_path_setup(struct path *path, unsigned len)
{
    size_t size = path->state_offset[path->nstates] * len;
    path->bit = imm_bitmap_reallocf(path->bit, size);
    if (!path->bit && size > 0) return error(IMM_NOMEM);
    return IMM_OK;
}
