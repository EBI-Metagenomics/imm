#include "dp_trans_table.h"
#include "dp_inctrans.h"
#include "hmm.h"
#include "imm/imm.h"
#include "list.h"
#include "state_idx.h"
#include "std.h"
#include <stdlib.h>

struct incoming_trans
{
    uint16_t         source_state;
    imm_float        score;
    struct list_head list_entry;
};

struct dp_trans_table_chunk
{
    uint16_t   ntrans;
    imm_float* score;
    uint16_t*  source_state;
    uint16_t   offset_size;
    uint16_t*  offset;
};

static inline unsigned offset_size(unsigned nstates) { return nstates + 1; }
static inline unsigned score_size(unsigned ntrans) { return ntrans; }
static inline unsigned source_state_size(unsigned ntrans) { return ntrans; }

int dp_trans_table_change(struct dp_trans_table* trans_tbl, unsigned src_state, unsigned tgt_state, imm_float lprob)
{
    /* TODO: find a faster way to update the transition */
    for (unsigned i = 0; i < dp_trans_table_ntrans(trans_tbl, tgt_state); ++i) {
        if (dp_trans_table_source_state(trans_tbl, tgt_state, i) == src_state) {

            trans_tbl->score[trans_tbl->offset[tgt_state] + i] = lprob;
            return IMM_SUCCESS;
        }
    }
    return 1;
}

struct dp_trans_table* dp_trans_table_create(struct imm_hmm const* hmm, struct imm_state** states,
                                             struct state_idx* state_idx)
{
    struct dp_trans_table* tbl = NULL;
    struct dp_inctrans*    inctrans = dp_inctrans_create(hmm, states, state_idx);
    if (!inctrans)
        goto err;

    tbl = xmalloc(sizeof(*tbl));
    tbl->ntrans = hmm->ntrans;
    tbl->offset = malloc(sizeof(*tbl->offset) * offset_size(hmm->nstates));
    if (!tbl->offset) {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }
    tbl->offset[0] = 0;

    if (tbl->ntrans > 0) {
        tbl->score = malloc(sizeof(*tbl->score) * score_size(tbl->ntrans));
        tbl->source_state = malloc(sizeof(*tbl->source_state) * source_state_size(tbl->ntrans));
        if (!tbl->score || !tbl->source_state) {
            error_explain(IMM_OUTOFMEM);
            goto err;
        }
    } else {
        tbl->score = NULL;
        tbl->source_state = NULL;
    }

    for (unsigned i = 0; i < hmm->nstates; ++i) {
        uint16_t         j = 0;
        struct inctrans* it = NULL;
        list_for_each_entry (it, inctrans->lhead_mem + i, list_entry) {

            tbl->score[tbl->offset[i] + j] = it->score;
            tbl->source_state[tbl->offset[i] + j] = it->source_state;
            ++j;
        }
        tbl->offset[i + 1] = tbl->offset[i] + j;
    }

    dp_inctrans_destroy(inctrans);
    return tbl;

err:
    if (inctrans)
        dp_inctrans_destroy(inctrans);

    if (tbl) {
        free_if(tbl->score);
        free_if(tbl->source_state);
        free(tbl);
    }
    return NULL;
}

void dp_trans_table_destroy(struct dp_trans_table const* trans_tbl)
{
    free(trans_tbl->score);
    free(trans_tbl->source_state);
    free(trans_tbl->offset);
    free((void*)trans_tbl);
}

struct dp_trans_table* dp_trans_table_read(FILE* stream)
{
    struct dp_trans_table_chunk chunk = {
        .ntrans = 0, .score = NULL, .source_state = NULL, .offset_size = 0, .offset = NULL};

    struct dp_trans_table* trans_tbl = xmalloc(sizeof(*trans_tbl));

    if (fread(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1) {
        error("could not read ntransitions");
        goto err;
    }

    chunk.score = malloc(sizeof(*chunk.score) * score_size(chunk.ntrans));
    if (!chunk.score) {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.score, sizeof(*chunk.score), score_size(chunk.ntrans), stream) < score_size(chunk.ntrans)) {
        error("could not read score");
        goto err;
    }

    chunk.source_state = malloc(sizeof(*chunk.source_state) * source_state_size(chunk.ntrans));
    if (!chunk.source_state) {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.source_state, sizeof(*chunk.source_state), source_state_size(chunk.ntrans), stream) <
        source_state_size(chunk.ntrans)) {
        error("could not read source_state");
        goto err;
    }

    if (fread(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        error("could not read offset_size");
        goto err;
    }

    chunk.offset = malloc(sizeof(*chunk.offset) * chunk.offset_size);
    if (!chunk.offset) {
        error_explain(IMM_OUTOFMEM);
        goto err;
    }

    if (fread(chunk.offset, sizeof(*chunk.offset), chunk.offset_size, stream) < chunk.offset_size) {
        error("could not read offset");
        goto err;
    }

    trans_tbl->ntrans = chunk.ntrans;
    trans_tbl->score = chunk.score;
    trans_tbl->source_state = chunk.source_state;
    trans_tbl->offset = chunk.offset;

    return trans_tbl;

err:
    free_if(chunk.score);
    free_if(chunk.source_state);
    free_if(chunk.offset);
    free(trans_tbl);
    return NULL;
}

int dp_trans_table_write(struct dp_trans_table const* trans, unsigned nstates, FILE* stream)
{
    struct dp_trans_table_chunk chunk = {.ntrans = trans->ntrans,
                                         .score = trans->score,
                                         .source_state = trans->source_state,
                                         .offset_size = (uint16_t)(nstates + 1),
                                         .offset = trans->offset};

    if (fwrite(&chunk.ntrans, sizeof(chunk.ntrans), 1, stream) < 1) {
        error("could not write ntransitions");
        return 1;
    }

    if (fwrite(chunk.score, sizeof(*chunk.score), score_size(trans->ntrans), stream) < score_size(trans->ntrans)) {
        error("could not write score");
        return 1;
    }

    if (fwrite(chunk.source_state, sizeof(*chunk.source_state), source_state_size(trans->ntrans), stream) <
        source_state_size(trans->ntrans)) {
        error("could not write source_state");
        return 1;
    }

    if (fwrite(&chunk.offset_size, sizeof(chunk.offset_size), 1, stream) < 1) {
        error("could not write offset_size");
        return 1;
    }

    if (fwrite(chunk.offset, sizeof(*chunk.offset), offset_size(nstates), stream) < offset_size(nstates)) {
        error("could not write offset");
        return 1;
    }

    return 0;
}
