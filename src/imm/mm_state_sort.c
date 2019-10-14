#include "src/imm/mm_state_sort.h"
#include "imm.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_trans.h"
#include "src/imm/state_idx.h"
#include "src/uthash/uthash.h"
#include "src/uthash/utlist.h"
#include <stddef.h>
#include <stdlib.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct edge;

struct node
{
    const struct imm_state *state;
    const struct mm_state *mm_state;
    int idx;
    int mark;
    struct edge *edges;
    struct node *next;
    struct node *prev;
};

struct edge
{
    struct node *node;
    struct edge *next;
};

struct state_node
{
    const struct imm_state *state;
    struct node *node;
    UT_hash_handle hh;
};

HIDE void create_nodes(const struct mm_state *head, struct node **nodes,
                       struct state_node **state_nodes);
HIDE void destroy_nodes(struct node **nodes);
HIDE void destroy_node(struct node *node);
HIDE void create_edges(struct node *nodes, struct state_node *state_nodes);
HIDE void destroy_edges(struct edge **edges);
HIDE void visit(struct node **node_head, struct node *node, const struct mm_state ***mm_state);
HIDE int check_mute_cycles(struct node *node_head);
HIDE int check_mute_visit(struct node *node);
HIDE void unmark_nodes(struct node *node_head);

int mm_state_sort(struct mm_state *const *head_ptr)
{
    struct node *node_head = NULL;
    struct state_node *state_node_head = NULL;

    create_nodes(*head_ptr, &node_head, &state_node_head);
    int nstates = mm_state_nitems(*head_ptr);

    if (check_mute_cycles(node_head))
        return 1;
    unmark_nodes(node_head);

    const struct mm_state **mm_state = malloc(sizeof(struct mm_state *) * (size_t)nstates);
    const struct mm_state **cur = mm_state;
    while (node_head) {
        visit(&node_head, node_head, &cur);
    }

    destroy_nodes(&node_head);

    return 0;
}

void create_nodes(const struct mm_state *head, struct node **nodes,
                  struct state_node **state_nodes)
{
    *state_nodes = NULL;

    const struct mm_state *mm_state = head;
    while (mm_state) {

        struct node *node = malloc(sizeof(struct node));
        node->state = mm_state_get_state(mm_state);
        node->mm_state = mm_state;
        node->mark = INITIAL_MARK;
        node->edges = NULL;
        DL_PREPEND(*nodes, node);

        struct state_node *state_node = malloc(sizeof(struct state_node));
        state_node->state = node->state;
        state_node->node = node;
        HASH_ADD_PTR(*state_nodes, state, state_node);

        mm_state = mm_state_next_c(mm_state);
    }

    create_edges(*nodes, *state_nodes);
}

void destroy_nodes(struct node **nodes)
{
    struct node *node = NULL, *tmp = NULL;
    DL_FOREACH_SAFE(*nodes, node, tmp)
    {
        DL_DELETE(*nodes, node);
        destroy_node(node);
    }
}

void destroy_node(struct node *node)
{
    destroy_edges(&node->edges);
    node->state = NULL;
    node->mm_state = NULL;
    node->idx = -1;
    node->mark = INITIAL_MARK;
    free(node);
}

void create_edges(struct node *nodes, struct state_node *state_nodes)
{
    struct node *node = NULL;
    DL_FOREACH(nodes, node)
    {
        const struct mm_trans *trans = mm_state_get_trans_c(node->mm_state);
        while (trans) {
            struct edge *edge = malloc(sizeof(struct edge));
            const struct state_node *sn = NULL;
            const struct imm_state *state = mm_trans_get_state(trans);
            HASH_FIND_PTR(state_nodes, &state, sn);
            edge->node = sn->node;
            LL_PREPEND(node->edges, edge);

            trans = mm_trans_next_c(trans);
        }
    }
}

void destroy_edges(struct edge **edges)
{
    struct edge *edge = NULL, *tmp = NULL;
    LL_FOREACH_SAFE(*edges, edge, tmp)
    {
        edge->node = NULL;
        LL_DELETE(*edges, edge);
        free(edge);
    }
}

void visit(struct node **node_head, struct node *node, const struct mm_state ***mm_state)
{
    if (node->mark == PERMANENT_MARK)
        return;
    if (node->mark == TEMPORARY_MARK)
        return;

    node->mark = TEMPORARY_MARK;
    const struct edge *edge = NULL;
    LL_FOREACH(node->edges, edge)
    {
        visit(node_head, edge->node, mm_state);
    }
    node->mark = PERMANENT_MARK;
    **mm_state = node->mm_state;
    *mm_state += 1;
    DL_DELETE(*node_head, node);
}

int check_mute_cycles(struct node *node_head)
{
    struct node *node = NULL;
    DL_FOREACH(node_head, node)
    {
        if (check_mute_visit(node))
            return 1;
    }
    return 0;
}

int check_mute_visit(struct node *node)
{
    if (imm_state_min_seq(node->state) > 0)
        return 0;

    if (node->mark == PERMANENT_MARK)
        return 0;

    if (node->mark == TEMPORARY_MARK) {
        imm_error("mute cycles are not allowed");
        return 1;
    }

    node->mark = TEMPORARY_MARK;
    const struct edge *edge = NULL;
    LL_FOREACH(node->edges, edge)
    {
        if (check_mute_visit(edge->node))
            return 1;
    }
    node->mark = PERMANENT_MARK;

    return 0;
}

void unmark_nodes(struct node *node_head)
{
    struct node *node = NULL;
    DL_FOREACH(node_head, node) { node->mark = INITIAL_MARK; }
}
