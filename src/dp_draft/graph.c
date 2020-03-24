#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include <stdlib.h>

struct edge
{
    struct mtrans const* trans;
    unsigned             state;
};

struct node
{
    struct edge const* edges;
    unsigned           nedges;
};

struct graph
{
    struct node const*          nodes;
    struct mstate const* const* mstates;
    unsigned                    nstates;
};

struct graph* graph_create(struct mstate const* const* mstates, unsigned nstates)
{
    struct graph* graph = malloc(sizeof(struct graph));

    struct node* nodes = malloc(sizeof(struct node) * nstates);
    for (unsigned i = 0; i < nstates; ++i) {

        struct mtrans_table* tbl = mstate_get_mtrans_table(mstates[i]);
        nodes[i].nedges = mtrans_table_size(tbl);
        struct edge * edges = malloc(sizeof(struct edge) * nodes[i].nedges);

        unsigned long iter = 0;
        unsigned j = 0;
        mtrans_table_for_each(iter, tbl) {
            if (!mtrans_table_exist(tbl, iter))
                continue;

            edges[j].trans = mtrans_table_get(tbl, iter);
            /* edges[j].state = i; */
        }

        nodes[i].edges = edges;
    }

    graph->nodes = nodes;

    return graph;
}
