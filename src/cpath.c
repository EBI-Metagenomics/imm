#include "cpath.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"

void cpath_init(struct cpath* path, struct dp_state_table const* state_tbl, struct dp_trans_table const* trans_tbl)
{
    path->nstates = dp_state_table_nstates(state_tbl);
    path->bits_state = malloc(sizeof(*path->bits_state) * (path->nstates + 1));
    path->bits_trans = malloc(sizeof(*path->bits_trans) * path->nstates);
    path->bits_state[0] = 0;

    for (uint_fast16_t tgt = 0; tgt < path->nstates; ++tgt) {

        uint_fast8_t depth = 0;
        for (uint_fast16_t i = 0; i < dp_trans_table_ntrans(trans_tbl, tgt); ++i) {

            uint_fast16_t src = dp_trans_table_source_state(trans_tbl, tgt, i);
            uint_fast8_t  min_seq = dp_state_table_min_seq(state_tbl, src);
            uint_fast8_t  max_seq = dp_state_table_max_seq(state_tbl, src);
            depth = (uint_fast8_t)MAX(max_seq - min_seq, depth);
        }
        path->bits_state[tgt + 1] = path->bits_state[tgt];
        path->bits_trans[tgt] = (uint8_t)bits_needed(dp_trans_table_ntrans(trans_tbl, tgt));
        path->bits_state[tgt + 1] += path->bits_trans[tgt];
        /* Additional bit (if necessary) for invalid transition or seq_len */
        path->bits_state[tgt + 1] += bits_needed(depth + 1);
    }
    path->bitarr = NULL;
}

void cpath_deinit(struct cpath const* path)
{
    free(path->bits_state);
    free(path->bits_trans);
    if (path->bitarr)
        free(path->bitarr);
}

void cpath_setup(struct cpath* path, uint32_t len)
{
    uint64_t size = (uint64_t)path->bits_state[path->nstates] * (uint64_t)len;
    path->bitarr = bitarr_realloc(path->bitarr, size);
}
