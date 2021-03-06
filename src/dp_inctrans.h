#ifndef DP_INCTRANS_H
#define DP_INCTRANS_H

#include "imm/imm.h"
#include "list.h"
#include <stdint.h>

struct imm_hmm;
struct imm_state;
struct state_idx;

struct inctrans
{
    uint16_t         source_state;
    imm_float        score;
    struct list_head list_entry;
};

struct dp_inctrans
{
    uint16_t          ntotal_trans;
    struct inctrans*  itrans_mem;
    struct list_head* lhead_mem;
    struct list_head* next_list;
};

struct dp_inctrans* dp_inctrans_create(struct imm_hmm const* hmm, struct imm_state** states,
                                       struct state_idx const* state_idx);
void                dp_inctrans_destroy(struct dp_inctrans const* inctrans);

#endif
