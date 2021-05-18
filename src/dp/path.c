#include "dp/path.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "support.h"

void path_init(struct path *path, struct state_table const *state_tbl,
               struct trans_table const *trans_tbl)
{
    path->state_offset = NULL;
    path->trans_bits = NULL;
    path_reset(path, state_tbl, trans_tbl);
    path->bit = NULL;
}

void path_reset(struct path *path, struct state_table const *state_tbl,
                struct trans_table const *trans_tbl)
{
    path->nstates = state_tbl->nstates;
    path->state_offset = xrealloc(
        path->state_offset, sizeof(*path->state_offset) * (path->nstates + 1));
    path->trans_bits =
        xrealloc(path->trans_bits, sizeof(*path->trans_bits) * path->nstates);
    path->state_offset[0] = 0;

    for (unsigned dst = 0; dst < path->nstates; ++dst)
    {
        unsigned depth = 0;
        for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst); ++i)
        {

            unsigned src = trans_table_source_state(trans_tbl, dst, i);
            unsigned min_seq = state_table_span(state_tbl, src).min;
            unsigned max_seq = state_table_span(state_tbl, src).max;
            depth = MAX(max_seq - min_seq, depth);
        }
        path->state_offset[dst + 1] = path->state_offset[dst];
        path->trans_bits[dst] =
            (uint8_t)bits_width((uint32_t)trans_table_ntrans(trans_tbl, dst));
        path->state_offset[dst + 1] =
            (uint16_t)(path->state_offset[dst + 1] + path->trans_bits[dst]);
        /* Additional bit (if necessary) for invalid transition or seq_len */
        path->state_offset[dst + 1] =
            (uint16_t)(path->state_offset[dst + 1] +
                       bits_width((uint32_t)((unsigned)depth + 1)));
    }
}

void path_del(struct path const *path)
{
    free(path->state_offset);
    free(path->trans_bits);
    if (path->bit)
        free(path->bit);
}

void path_setup(struct path *path, unsigned len)
{
    size_t size = path->state_offset[path->nstates] * len;
    path->bit = bitmap_realloc(path->bit, size);
}
