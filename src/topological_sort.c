#include "topological_sort.h"
#include "containers/containers.h"
#include "hmm.h"
#include "std.h"
#include <stdlib.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct vert
{
    struct imm_state* state;
    int               mark;
    struct queue      edgeq;
    struct node       node;
    struct hnode      hnode;
};

struct edge
{
    struct vert* vert;
    struct node  node;
};

struct graph
{
    struct vert* verts;
    struct edge* edges;
    struct queue vertq;
    HASH_DECLARE(vert_tbl, 11);
};

static int  check_mute_cycles(struct queue* vertq);
static int  check_mute_visit(struct vert* vert);
static void create_edges(struct graph* graph);
static void create_vertices(struct graph* graph, struct imm_state** states, uint16_t nstates);
static void graph_deinit(struct graph const* graph);
static void graph_init(struct graph* graph, uint16_t nstates, uint16_t ntrans);
static void unmark_nodes(struct queue* vertq);
static void visit(struct vert* vert, struct imm_state*** state);

int topological_sort(struct imm_state** states, uint16_t nstates, uint16_t ntrans)
{
    struct graph graph;
    graph_init(&graph, nstates, ntrans);

    create_vertices(&graph, states, nstates);

    if (check_mute_cycles(&graph.vertq)) {
        error("mute cycles are not allowed");
        graph_deinit(&graph);
        return IMM_RUNTIMEERROR;
    }
    unmark_nodes(&graph.vertq);

    /* TODO: could we use the original state array instead? */
    struct imm_state** state_arr = xmalloc(sizeof(*state_arr) * nstates);
    struct imm_state** cur = state_arr + nstates;
    struct vert*       vert = NULL;
    struct iter        iter = queue_iter(&graph.vertq);
    iter_for_each_entry(vert, &iter, node) { visit(vert, &cur); }

    for (uint16_t i = 0; i < nstates; ++i)
        states[i] = state_arr[i];

    free(state_arr);
    graph_deinit(&graph);
    return IMM_SUCCESS;
}

static int check_mute_cycles(struct queue* vertq)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node)
    {
        if (check_mute_visit(vert))
            return IMM_FAILURE;
    }
    return IMM_SUCCESS;
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
    struct iter  iter0 = queue_iter(&graph->vertq);
    struct edge* edge = graph->edges;
    iter_for_each_entry(src, &iter0, node)
    {
        struct trans* trans = NULL;
        struct iter   iter1 = stack_iter(&src->state->trans);
        iter_for_each_entry(trans, &iter1, node)
        {
            struct vert* dst = NULL;
            uint16_t     id = trans->pair.ids[1];
            hash_for_each_possible(graph->vert_tbl, dst, hnode, id)
            {
                if (dst->state->id == id)
                    break;
            }
            edge->vert = dst;
            queue_put(&src->edgeq, &edge->node);
            ++edge;
        }
    }
}

static void create_vertices(struct graph* graph, struct imm_state** states, uint16_t nstates)
{
    for (uint16_t i = 0; i < nstates; ++i) {
        struct vert* vert = graph->verts + i;
        vert->state = states[i];
        vert->mark = INITIAL_MARK;

        queue_init(&vert->edgeq);
        queue_put_first(&graph->vertq, &vert->node);

        hash_add(graph->vert_tbl, &vert->hnode, vert->state->id);
    }

    create_edges(graph);
}

static void graph_deinit(struct graph const* graph)
{
    free(graph->verts);
    free(graph->edges);
}

static void graph_init(struct graph* graph, uint16_t nstates, uint16_t ntrans)
{
    graph->verts = xmalloc(sizeof(*graph->verts) * nstates);
    graph->edges = xmalloc(sizeof(*graph->edges) * ntrans);
    graph->edges = xmalloc(sizeof(*graph->edges) * ntrans);
    queue_init(&graph->vertq);
    hash_init(graph->vert_tbl);
}

static void unmark_nodes(struct queue* vertq)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node) { vert->mark = INITIAL_MARK; }
}

static void visit(struct vert* vert, struct imm_state*** state)
{
    if (vert->mark == PERMANENT_MARK)
        return;
    if (vert->mark == TEMPORARY_MARK)
        return;

    vert->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    struct iter        iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node) { visit(edge->vert, state); }
    vert->mark = PERMANENT_MARK;
    *state -= 1;
    **state = vert->state;
}
