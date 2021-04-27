#include "dp/trans_table.h"
#include "common/common.h"
#include "containers/iter.h"
#include "containers/queue.h"
#include "trans.h"
#include <stdlib.h>

struct chunk
{
    uint16_t ntrans;
    imm_float *score;
    stateidx_t *src;
    uint16_t offset_size;
    uint16_t *offset;
};

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }
static inline unsigned src_state_size(unsigned ntrans) { return ntrans; }

int trans_table_change(struct trans_table *trans_tbl, unsigned src,
                       unsigned dst, imm_float lprob)
{
    /* TODO: find a faster way to update the transition */
    for (unsigned i = 0; i < trans_table_ntrans(trans_tbl, dst); ++i)
    {
        if (trans_table_source_state(trans_tbl, dst, i) == src)
        {

            trans_tbl->score[trans_tbl->offset[dst] + i] = lprob;
            return IMM_SUCCESS;
        }
    }
    return 1;
}

struct trans_table *trans_table_new(struct dp_args const *args)
{
    struct trans_table *tbl = xmalloc(sizeof(*tbl));
    tbl->ntrans = args->ntransitions;
    tbl->offset = xmalloc(sizeof(*tbl->offset) * offset_size(args->nstates));
    tbl->offset[0] = 0;

    if (tbl->ntrans > 0)
    {
        tbl->score = xmalloc(sizeof(*tbl->score) * tbl->ntrans);
        tbl->src = xmalloc(sizeof(*tbl->src) * tbl->ntrans);
    }
    else
    {
        tbl->score = NULL;
        tbl->src = NULL;
    }

    for (unsigned i = 0; i < args->nstates; ++i)
    {
        struct iter it = stack_iter(&args->states[i]->trans.incoming);
        struct trans *trans = NULL;
        unsigned j = 0;
        iter_for_each_entry(trans, &it, inode)
        {
            tbl->score[tbl->offset[i] + j] = trans->lprob;
            tbl->src[tbl->offset[i] + j] = trans->pair.idx.src;
            ++j;
        }
        tbl->offset[i + 1] = (uint16_t)(tbl->offset[i] + j);
    }
    return tbl;
}

void trans_table_destroy(struct trans_table const *tbl)
{
    free(tbl->score);
    free(tbl->src);
    free(tbl->offset);
    free((void *)tbl);
}

#if 0
struct trans_table *trans_table_read(FILE *stream)
{
    struct chunk chunk = {.ntrans = 0,
                          .score = NULL,
                          .src_state = NULL,
                          .offset_size = 0,
                          .offset = NULL};

    struct trans_table *trans_tbl = xmalloc(sizeof(*trans_tbl));

    if (fread(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1)
    {
        error("could not read ntransitions");
        goto err;
    }

    chunk.score = malloc(sizeof(*chunk.score) * score_size(chunk.ntrans));
    if (!chunk.score)
    {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.score, sizeof(*chunk.score), score_size(chunk.ntrans),
              stream) < score_size(chunk.ntrans))
    {
        error("could not read score");
        goto err;
    }

    chunk.src_state =
        malloc(sizeof(*chunk.src_state) * src_state_size(chunk.ntrans));
    if (!chunk.src_state)
    {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.src_state, sizeof(*chunk.src_state),
              src_state_size(chunk.ntrans),
              stream) < src_state_size(chunk.ntrans))
    {
        error("could not read src_state");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1)
    {
        error("could not read offset_size");
        goto err;
    }

    chunk.offset = malloc(sizeof(*chunk.offset) * chunk.offset_size);
    if (!chunk.offset)
    {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.offset, sizeof(*chunk.offset), chunk.offset_size, stream) <
        chunk.offset_size)
    {
        error("could not read offset");
        goto err;
    }

    trans_tbl->ntrans = chunk.ntrans;
    trans_tbl->score = chunk.score;
    trans_tbl->src_state = chunk.src_state;
    trans_tbl->offset = chunk.offset;

    return trans_tbl;

err:
    free_if(chunk.score);
    free_if(chunk.src_state);
    free_if(chunk.offset);
    free(trans_tbl);
    return NULL;
}

int trans_table_write(struct trans_table const *trans, unsigned nstates,
                         FILE *stream)
{
    struct chunk chunk = {.ntrans = trans->ntrans,
                          .score = trans->score,
                          .src_state = trans->src_state,
                          .offset_size = (uint16_t)(nstates + 1),
                          .offset = trans->offset};

    if (fwrite(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1)
    {
        error("could not write ntransitions");
        return 1;
    }

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(trans->ntrans),
               stream) < score_size(trans->ntrans))
    {
        error("could not write score");
        return 1;
    }

    if (fwrite(chunk.src_state, sizeof(*chunk.src_state),
               src_state_size(trans->ntrans),
               stream) < src_state_size(trans->ntrans))
    {
        error("could not write src_state");
        return 1;
    }

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1)
    {
        error("could not write offset_size");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates),
               stream) < offset_size(nstates))
    {
        error("could not write offset");
        return 1;
    }

    return 0;
}
#endif
