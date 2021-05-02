#include "tsort.h"
#include "common/common.h"
#include "containers/containers.h"
#include "imm/state.h"
#include "trans.h"
#include <containers/hash.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct vert
{
    struct imm_state *state;
    int mark;
    struct queue edgeq;
    struct node node;
    struct hnode hnode;
};

struct edge
{
    struct vert *vert;
    struct node node;
};

struct graph
{
    struct vert *verts;
    struct edge *edges;
    struct queue vertq;
    HASH_DECLARE(vert_tbl, 11);
};

static int check_mute_cycles(struct queue *vertq);
static int check_mute_visit(struct vert *vert);
static void create_edges(struct graph *graph);
static void create_vertices(struct graph *graph, unsigned nstates,
                            struct imm_state **states);
static void graph_deinit(struct graph const *graph);
static void graph_init(struct graph *graph, unsigned nstates, unsigned ntrans);
static void unmark_nodes(struct queue *vertq);
static void visit(struct vert *vert, struct imm_state **state, unsigned *end);

int tsort(unsigned nstates, struct imm_state **states, unsigned start_state,
          unsigned ntrans)
{
    struct graph graph;
    graph_init(&graph, nstates, ntrans);

    create_vertices(&graph, nstates, states);

    if (check_mute_cycles(&graph.vertq))
    {
        error("mute cycles are not allowed");
        graph_deinit(&graph);
        return IMM_RUNTIMEERROR;
    }
    unmark_nodes(&graph.vertq);

    /* TODO: could we use the original state array instead? */
    struct imm_state **state_arr = xmalloc(sizeof(*state_arr) * nstates);
    /* TODO: nulling only needed for debugging */
    for (unsigned i = 0; i < nstates; ++i)
        state_arr[i] = NULL;

    struct vert *vert = NULL;

    unsigned end = nstates;
    hash_for_each_possible(graph.vert_tbl, vert, hnode, start_state)
    {
        if (vert->state->id == start_state)
        {
            visit(vert, state_arr, &end);
            break;
        }
    }

    unsigned bkt = 0;
    hash_for_each(graph.vert_tbl, bkt, vert, hnode)
    {
        if (vert->state->id != start_state)
        {
            visit(vert, state_arr, &end);
            break;
        }
    }

    for (unsigned i = 0; i < nstates; ++i)
        states[i] = state_arr[i];

    free(state_arr);
    graph_deinit(&graph);
    return IMM_SUCCESS;
}

static int check_mute_cycles(struct queue *vertq)
{
    struct vert *vert = NULL;
    struct iter iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node)
    {
        if (check_mute_visit(vert))
            return IMM_FAILURE;
    }
    return IMM_SUCCESS;
}

static int check_mute_visit(struct vert *vert)
{
    if (imm_state_min_seqlen(vert->state) > 0)
        return 0;

    if (vert->mark == PERMANENT_MARK)
        return 0;

    if (vert->mark == TEMPORARY_MARK)
    {
        return 1;
    }

    vert->mark = TEMPORARY_MARK;
    struct edge const *edge = NULL;
    struct iter iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node)
    {
        if (check_mute_visit(edge->vert))
            return 1;
    }
    vert->mark = PERMANENT_MARK;

    return 0;
}

static void create_edges(struct graph *graph)
{
    struct vert *src = NULL;
    struct iter iter0 = queue_iter(&graph->vertq);
    struct edge *edge = graph->edges;
    iter_for_each_entry(src, &iter0, node)
    {
        struct trans *trans = NULL;
        struct iter iter1 = stack_iter(&src->state->trans.outgoing);
        iter_for_each_entry(trans, &iter1, outgoing)
        {
            struct vert *dst = NULL;
            unsigned id = trans->pair.id.dst;
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

static void create_vertices(struct graph *graph, unsigned nstates,
                            struct imm_state **states)
{
    for (unsigned i = 0; i < nstates; ++i)
    {
        struct vert *vert = graph->verts + i;
        vert->state = states[i];
        vert->mark = INITIAL_MARK;

        queue_init(&vert->edgeq);
        queue_put_first(&graph->vertq, &vert->node);

        hash_add(graph->vert_tbl, &vert->hnode, vert->state->id);
    }

    create_edges(graph);
}

static void graph_deinit(struct graph const *graph)
{
    free(graph->verts);
    free(graph->edges);
}

static void graph_init(struct graph *graph, unsigned nstates, unsigned ntrans)
{
    graph->verts = xmalloc(sizeof(*graph->verts) * nstates);
    graph->edges = xmalloc(sizeof(*graph->edges) * ntrans);
    queue_init(&graph->vertq);
    hash_init(graph->vert_tbl);
}

static void unmark_nodes(struct queue *vertq)
{
    struct vert *vert = NULL;
    struct iter iter = queue_iter(vertq);
    iter_for_each_entry(vert, &iter, node) { vert->mark = INITIAL_MARK; }
}

static void visit(struct vert *vert, struct imm_state **state, unsigned *end)
{
    if (vert->mark == PERMANENT_MARK)
        return;
    if (vert->mark == TEMPORARY_MARK)
        return;

    vert->mark = TEMPORARY_MARK;
    struct edge const *edge = NULL;
    struct iter iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node) { visit(edge->vert, state, end); }
    vert->mark = PERMANENT_MARK;
    *end = *end - 1;
    state[*end] = vert->state;
}
