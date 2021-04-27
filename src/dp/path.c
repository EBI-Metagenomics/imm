#include "dp/path.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"

void path_init(struct path *path, struct dp_state_table const *state_tbl,
               struct dp_trans_table const *trans_tbl)
{
    path->nstates = state_tbl->nstates;
    path->state_offset =
        malloc(sizeof(*path->state_offset) * path->nstates + 1);
    path->trans_bits = malloc(sizeof(*path->trans_bits) * path->nstates);
    path->state_offset[0] = 0;

    for (unsigned tgt = 0; tgt < path->nstates; ++tgt)
    {

        unsigned depth = 0;
        for (unsigned i = 0; i < dp_trans_table_ntrans(trans_tbl, tgt); ++i)
        {

            unsigned src = dp_trans_table_source_state(trans_tbl, tgt, i);
            unsigned min_seq = dp_state_table_min_seq(state_tbl, src);
            unsigned max_seq = dp_state_table_max_seq(state_tbl, src);
            depth = MAX(max_seq - min_seq, depth);
        }
        path->state_offset[tgt + 1] = path->state_offset[tgt];
        path->trans_bits[tgt] = (uint8_t)bits_width(
            (uint32_t)dp_trans_table_ntrans(trans_tbl, tgt));
        path->state_offset[tgt + 1] =
            (uint16_t)(path->state_offset[tgt + 1] + path->trans_bits[tgt]);
        /* Additional bit (if necessary) for invalid transition or seq_len */
        path->state_offset[tgt + 1] =
            (uint16_t)(path->state_offset[tgt + 1] +
                       bits_width((uint32_t)((unsigned)depth + 1)));
    }
    path->bit = NULL;
}

void path_deinit(struct path const *path)
{
    free(path->state_offset);
    free(path->trans_bits);
    if (path->bit)
        free(path->bit);
}

int path_setup(struct path *path, unsigned len)
{
    size_t size = path->state_offset[path->nstates] * len;
    path->bit = bitarr_realloc(path->bit, size);
    return IMM_SUCCESS;
}
