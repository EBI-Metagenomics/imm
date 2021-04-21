#include "dp_inctrans.h"
#include "hmm.h"
#include "imm/imm.h"
#include "list.h"
#include "state_idx.h"
#include "std.h"
#include <stdint.h>
#include <stdlib.h>

static struct list_head* create_inctrans(unsigned nstates);

struct dp_inctrans* dp_inctrans_create(struct imm_hmm const* hmm, struct imm_state** states,
                                       struct state_idx const* state_idx)

{
    struct dp_inctrans* inctrans = xmalloc(sizeof(*inctrans));
    inctrans->itrans_mem = xmalloc(sizeof(*inctrans->itrans_mem) * hmm->ntrans);

    inctrans->lhead_mem = create_inctrans(hmm->nstates);
    if (!inctrans->lhead_mem) {
        error_explain(IMM_OUTOFMEM);
        free(inctrans->itrans_mem);
        free(inctrans);
        return NULL;
    }

    struct inctrans*  it = inctrans->itrans_mem;
    struct list_head* list = inctrans->lhead_mem;
    for (unsigned i = 0; i < hmm->nstates; ++i) {

        printf("i: %d\n", i);
        struct imm_state* src = states[i];
        uint16_t          src_idx = state_idx_find(state_idx, src);
        struct iter       iter = stack_iter(&src->trans);
        struct trans*     trans = NULL;
        iter_for_each_entry(trans, &iter, node)
        {
            struct imm_state const* dst = hmm_state(hmm, trans->pair.ids[1]);
            uint16_t                dst_idx = state_idx_find(state_idx, dst);
            it->score = trans->lprob;
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

static struct list_head* create_inctrans(unsigned nstates)
{
    struct list_head* inctrans = xmalloc(sizeof(*inctrans) * nstates);
    for (unsigned i = 0; i < nstates; ++i)
        INIT_LIST_HEAD(inctrans + i);
    return inctrans;
}
