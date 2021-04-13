#include "cpath.h"
#include "dp_state_table.h"
#include "dp_trans_table.h"

void cpath_init(struct cpath* path, struct dp_state_table const* state_tbl, struct dp_trans_table const* trans_tbl)
{
    path->nstates = (uint16_t)dp_state_table_nstates(state_tbl);
    path->bits_state = malloc(sizeof(*path->bits_state) * (unsigned)(path->nstates + 1));
    path->bits_trans = malloc(sizeof(*path->bits_trans) * (unsigned)path->nstates);
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
        path->bits_trans[tgt] = (uint8_t)bits_width((uint32_t)dp_trans_table_ntrans(trans_tbl, tgt));
        path->bits_state[tgt + 1] = (uint16_t)(path->bits_state[tgt + 1] + path->bits_trans[tgt]);
        /* Additional bit (if necessary) for invalid transition or seq_len */
        path->bits_state[tgt + 1] = (uint16_t)(path->bits_state[tgt + 1] + bits_width((uint32_t)((unsigned)depth + 1)));
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

int cpath_setup(struct cpath* path, uint_fast32_t len)
{
    uint64_t size = (uint64_t)path->bits_state[path->nstates] * (uint64_t)len;
    void*    ptr = bitarr_realloc(path->bitarr, size);
    if (!ptr) {
        free(path->bitarr);
        path->bitarr = NULL;
        return IMM_OUTOFMEM;
    }
    path->bitarr = ptr;
    return IMM_SUCCESS;
}
