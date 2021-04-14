#include "dp_inctrans.h"
#include "imm/imm.h"
#include "list.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "state_idx.h"
#include "std.h"
#include <stdint.h>
#include <stdlib.h>

static struct list_head* create_inctrans(uint_fast16_t nstates);
static uint_fast16_t     trans_size(struct model_state const* const* mstates, uint_fast16_t nstates);

struct dp_inctrans* dp_inctrans_create(struct model_state const* const* mstates, uint_fast16_t nstates,
                                       struct state_idx const* state_idx)

{
    struct dp_inctrans* inctrans = xmalloc(sizeof(*inctrans));
    inctrans->ntotal_trans = trans_size(mstates, nstates);
    inctrans->itrans_mem = malloc(sizeof(*inctrans->itrans_mem) * inctrans->ntotal_trans);
    if (!inctrans->itrans_mem) {
        error("%s", explain(IMM_OUTOFMEM));
        free(inctrans);
        return NULL;
    }

    inctrans->lhead_mem = create_inctrans(nstates);
    if (!inctrans->lhead_mem) {
        error("%s", explain(IMM_OUTOFMEM));
        free(inctrans->itrans_mem);
        free(inctrans);
        return NULL;
    }

    struct inctrans*  it = inctrans->itrans_mem;
    struct list_head* list = inctrans->lhead_mem;
    for (uint_fast16_t i = 0; i < nstates; ++i) {

        struct imm_state const*         src = model_state_get_state(mstates[i]);
        uint16_t                        src_idx = state_idx_find(state_idx, src);
        struct model_trans_table const* table = model_state_get_mtrans_table(mstates[i]);

        unsigned long iter = 0;
        model_trans_table_for_each(iter, table)
        {
            if (!model_trans_table_exist(table, iter))
                continue;

            struct model_trans const* mtrans = model_trans_table_get(table, iter);
            struct imm_state const*   dst = model_trans_get_state(mtrans);
            uint_fast16_t             dst_idx = state_idx_find(state_idx, dst);

            it->score = model_trans_get_lprob(mtrans);
            it->source_state = src_idx;
            list_add_tail(&it->list_entry, list + dst_idx);
            ++it;
        }
    }
    inctrans->next_list = inctrans->lhead_mem;
    return inctrans;
}

void dp_inctrans_destroy(struct dp_inctrans const* inctrans)
{
    free(inctrans->itrans_mem);
    free(inctrans->lhead_mem);
    free((void*)inctrans);
}

static struct list_head* create_inctrans(uint_fast16_t nstates)
{
    struct list_head* inctrans = malloc(sizeof(*inctrans) * nstates);
    if (!inctrans)
        return NULL;

    for (uint_fast16_t i = 0; i < nstates; ++i)
        INIT_LIST_HEAD(inctrans + i);
    return inctrans;
}

static uint_fast16_t trans_size(struct model_state const* const* mstates, uint_fast16_t nstates)
{
    uint_fast16_t size = 0;
    for (uint_fast16_t i = 0; i < nstates; ++i) {

        struct model_trans_table const* table = model_state_get_mtrans_table(mstates[i]);
        BUG(model_trans_table_size(table) > UINT16_MAX - size);
        size += (uint_fast16_t)model_trans_table_size(table);
    }
    return size;
}
