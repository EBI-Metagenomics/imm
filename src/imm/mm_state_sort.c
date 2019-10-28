#include "src/imm/mm_state_sort.h"
#include "imm.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_trans.h"
#include "src/imm/state_idx.h"
#include "src/list.h"
#include "src/uthash/uthash.h"
#include <stddef.h>
#include <stdlib.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct edge;

struct node
{
    struct imm_state const* state;
    struct mm_state const*  mm_state;
    int                     idx;
    int                     mark;
    struct list_head        edges;
    struct list_head        list_entry;
};

struct edge
{
    struct node*     node;
    struct list_head list_entry;
};

struct state_node
{
    struct imm_state const* state;
    struct node*            node;
    UT_hash_handle          hh;
};

HIDE void create_nodes(struct mm_state const* head, struct list_head* nodes,
                       struct state_node** state_nodes);
HIDE void destroy_nodes(struct list_head* nodes);
HIDE void destroy_node(struct node* node);
HIDE void destroy_state_nodes(struct state_node** state_nodes);
HIDE void create_edges(struct list_head* nodes, struct state_node* state_nodes);
HIDE void destroy_edges(struct list_head* edges);
HIDE void visit(struct node* node, struct mm_state const*** mm_state);
HIDE int  check_mute_cycles(struct list_head* nodes);
HIDE int  check_mute_visit(struct node* node);
HIDE void unmark_nodes(struct list_head* nodes);

struct mm_state const* const* mm_state_sort(struct mm_state const* mm_state_head)
{
    struct state_node* state_node_head = NULL;
    struct list_head   nodes = LIST_HEAD_INIT(nodes);

    create_nodes(mm_state_head, &nodes, &state_node_head);

    if (check_mute_cycles(&nodes)) {
        destroy_nodes(&nodes);
        destroy_state_nodes(&state_node_head);
        return NULL;
    }
    unmark_nodes(&nodes);

    size_t                  nstates = (size_t)mm_state_nitems(mm_state_head);
    struct mm_state const** mm_state = malloc(sizeof(struct mm_state*) * nstates);
    struct mm_state const** cur = mm_state + nstates;
    struct node*            node = NULL;
    list_for_each_entry(node, &nodes, list_entry) { visit(node, &cur); }

    destroy_nodes(&nodes);
    destroy_state_nodes(&state_node_head);

    return mm_state;
}

void create_nodes(struct mm_state const* head, struct list_head* nodes,
                  struct state_node** state_nodes)
{
    *state_nodes = NULL;

    struct mm_state const* mm_state = head;
    while (mm_state) {

        struct node* node = malloc(sizeof(struct node));
        node->state = mm_state_get_state(mm_state);
        node->mm_state = mm_state;
        node->mark = INITIAL_MARK;
        INIT_LIST_HEAD(&node->edges);
        if (imm_lprob_is_zero(mm_state_get_start_lprob(node->mm_state)))
            list_add_tail(&node->list_entry, nodes);
        else
            list_add(&node->list_entry, nodes);

        struct state_node* state_node = malloc(sizeof(struct state_node));
        state_node->state = node->state;
        state_node->node = node;
        HASH_ADD_PTR(*state_nodes, state, state_node);

        mm_state = mm_state_next_c(mm_state);
    }

    create_edges(nodes, *state_nodes);
}

void destroy_nodes(struct list_head* nodes)
{
    struct node *node = NULL, *tmp = NULL;

    list_for_each_entry_safe(node, tmp, nodes, list_entry)
    {
        list_del(&node->list_entry);
        destroy_node(node);
    }
}

void destroy_node(struct node* node)
{
    destroy_edges(&node->edges);
    node->state = NULL;
    node->mm_state = NULL;
    node->idx = -1;
    node->mark = INITIAL_MARK;
    free(node);
}

void destroy_state_nodes(struct state_node** state_nodes)
{
    if (!*state_nodes)
        return;

    struct state_node *state_node = NULL, *tmp = NULL;

    if (*state_nodes) {
        HASH_ITER(hh, *state_nodes, state_node, tmp)
        {
            state_node->node = NULL;
            state_node->state = NULL;
            HASH_DEL(*state_nodes, state_node);
            free(state_node);
        }
    }
    *state_nodes = NULL;
}

void create_edges(struct list_head* nodes, struct state_node* state_nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        struct mm_trans const* trans = mm_state_get_trans_c(node->mm_state);
        while (trans) {
            if (!imm_lprob_is_zero(mm_trans_get_lprob(trans))) {
                struct edge*             edge = malloc(sizeof(struct edge));
                struct state_node const* sn = NULL;
                struct imm_state const*  state = mm_trans_get_state(trans);
                HASH_FIND_PTR(state_nodes, &state, sn);
                edge->node = sn->node;
                list_add(&edge->list_entry, &node->edges);
            }

            trans = mm_trans_next_c(trans);
        }
    }
}

void destroy_edges(struct list_head* edges)
{
    struct edge *edge = NULL, *tmp = NULL;
    list_for_each_entry_safe(edge, tmp, edges, list_entry)
    {
        edge->node = NULL;
        list_del(&edge->list_entry);
        free(edge);
    }
}

void visit(struct node* node, struct mm_state const*** mm_state)
{
    if (node->mark == PERMANENT_MARK)
        return;
    if (node->mark == TEMPORARY_MARK)
        return;

    node->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    list_for_each_entry(edge, &node->edges, list_entry) { visit(edge->node, mm_state); }
    node->mark = PERMANENT_MARK;
    *mm_state -= 1;
    **mm_state = node->mm_state;
}

int check_mute_cycles(struct list_head* nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        if (check_mute_visit(node))
            return 1;
    }
    return 0;
}

int check_mute_visit(struct node* node)
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
    struct edge const* edge = NULL;
    list_for_each_entry(edge, &node->edges, list_entry)
    {
        if (check_mute_visit(edge->node))
            return 1;
    }
    node->mark = PERMANENT_MARK;

    return 0;
}

void unmark_nodes(struct list_head* nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry) { node->mark = INITIAL_MARK; }
}
