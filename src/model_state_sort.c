#include "model_state_sort.h"
#include "containers/containers.h"
#include "imm/imm.h"
#include "log.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "std.h"
#include <stdlib.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct vert
{
    struct imm_state const*   state;
    struct model_state const* mstate;
    int                       mark;
    struct queue              edgeq;
    struct node               node;
    struct hnode              hnode;
};

struct edge
{
    struct vert* vert;
    struct node  node;
};

struct graph
{
    struct queue vertq;
    HASH_DECLARE(vert_tbl, 11);
};

static int        check_mute_cycles(struct queue* vertq);
static int        check_mute_visit(struct vert* vert);
static void       create_edges(struct graph* graph);
static void       create_nodes(struct graph* graph, struct model_state const** mstates, uint16_t nstates);
static inline int name_compare(void const* a, void const* b);
static void       unmark_nodes(struct queue* vertq);
static void       visit(struct vert* vert, struct model_state const*** mstate);

void model_state_name_sort(struct model_state const** mstates, uint16_t nstates)
{
    qsort(mstates, nstates, sizeof(*mstates), name_compare);
}

int model_state_topological_sort(struct model_state const** mstates, uint16_t nstates)
{
    struct graph graph;
    queue_init(&graph.vertq);
    hash_init(graph.vert_tbl);

    create_nodes(&graph, mstates, nstates);

    if (check_mute_cycles(&graph.vertq)) {
        error("mute cycles are not allowed");
        return IMM_ILLEGALARG;
    }
    unmark_nodes(&graph.vertq);

    struct model_state const** mstate_arr = malloc(sizeof(*mstate_arr) * nstates);
    struct model_state const** cur = mstate_arr + nstates;
    struct vert*               vert = NULL;
    struct iter                iter = queue_iter(&graph.vertq);
    iter_for_each_entry(vert, &iter, node) { visit(vert, &cur); }

    for (uint16_t i = 0; i < nstates; ++i)
        mstates[i] = mstate_arr[i];

    free(mstate_arr);
    return IMM_SUCCESS;
}

static int check_mute_cycles(struct queue* vertq)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node)
    {
        if (check_mute_visit(vert))
            return 1;
    }
    return 0;
}

static int check_mute_visit(struct vert* vert)
{
    if (imm_state_min_seq(vert->state) > 0)
        return 0;

    if (vert->mark == PERMANENT_MARK)
        return 0;

    if (vert->mark == TEMPORARY_MARK) {
        return 1;
    }

    vert->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    struct iter        iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node)
    {
        if (check_mute_visit(edge->vert))
            return 1;
    }
    vert->mark = PERMANENT_MARK;

    return 0;
}

static void create_edges(struct graph* graph)
{
    struct vert* src = NULL;
    struct iter  iter = queue_iter(&graph->vertq);
    iter_for_each_entry(src, &iter, node)
    {
        struct model_trans_table const* mtrans_table = model_state_get_mtrans_table(src->mstate);
        struct model_trans const**      mtrans = model_trans_table_array(mtrans_table);

        for (uint16_t i = 0; i < model_trans_table_size(mtrans_table); ++i) {

            struct model_trans const* t = mtrans[i];

            struct edge* edge = malloc(sizeof(*edge));

            struct vert* dst = NULL;
            uint16_t     key = imm_state_id(model_trans_get_state(t));
            hash_for_each_possible(graph->vert_tbl, dst, hnode, key)
            {
                if (imm_state_id(dst->state) == key)
                    break;
            }
            BUG(dst == NULL);

            edge->vert = dst;
            queue_put(&src->edgeq, &edge->node);
        }
        free(mtrans);
    }
}

static void create_nodes(struct graph* graph, struct model_state const** mstates, uint16_t nstates)
{
    for (uint16_t i = 0; i < nstates; ++i) {
        struct model_state const* mstate = mstates[i];

        struct vert* vert = malloc(sizeof(*vert));
        vert->state = model_state_get_state(mstate);
        vert->mstate = mstate;
        vert->mark = INITIAL_MARK;

        queue_init(&vert->edgeq);
        if (imm_lprob_is_zero(model_state_get_start(vert->mstate)))
            queue_put(&graph->vertq, &vert->node);
        else
            queue_put_first(&graph->vertq, &vert->node);

        hash_add(graph->vert_tbl, &vert->hnode, imm_state_id(vert->state));
    }

    create_edges(graph);
}

static inline int name_compare(void const* a, void const* b)
{
    struct imm_state const* s0 = (*(struct model_state const**)a)->state;
    struct imm_state const* s1 = (*(struct model_state const**)b)->state;
    return imm_state_id(s0) - imm_state_id(s1);
}

static void unmark_nodes(struct queue* vertq)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node) { vert->mark = INITIAL_MARK; }
}

static void visit(struct vert* vert, struct model_state const*** mstate)
{
    if (vert->mark == PERMANENT_MARK)
        return;
    if (vert->mark == TEMPORARY_MARK)
        return;

    vert->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    struct iter        iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node) { visit(edge->vert, mstate); }
    vert->mark = PERMANENT_MARK;
    *mstate -= 1;
    **mstate = vert->mstate;
}
