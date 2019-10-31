#include "mm_state_sort.h"
#include "imm/imm.h"
#include "list.h"
#include "mm_state.h"
#include "mm_trans.h"
#include "state_idx.h"
#include "uthash.h"
#include <stddef.h>
#include <stdlib.h>

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct edge;

struct node
{
    struct imm_state const* state;
    struct mstate const*    mm_state;
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

static void create_nodes(struct mstate const* head, struct list_head* nodes,
                         struct state_node** state_nodes);
static void destroy_nodes(struct list_head* nodes);
static void destroy_node(struct node* node);
static void destroy_state_nodes(struct state_node** state_nodes);
static void create_edges(struct list_head* nodes, struct state_node* state_nodes);
static void destroy_edges(struct list_head* edges);
static void visit(struct node* node, struct mstate const*** mm_state);
static int  check_mute_cycles(struct list_head* nodes);
static int  check_mute_visit(struct node* node);
static void unmark_nodes(struct list_head* nodes);

struct mstate const* const* imm_mstate_sort(struct mstate const* mm_state_head)
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

    size_t                nstates = (size_t)imm_mstate_nitems(mm_state_head);
    struct mstate const** mm_state = malloc(sizeof(struct mstate*) * nstates);
    struct mstate const** cur = mm_state + nstates;
    struct node*          node = NULL;
    list_for_each_entry(node, &nodes, list_entry) { visit(node, &cur); }

    destroy_nodes(&nodes);
    destroy_state_nodes(&state_node_head);

    return mm_state;
}

static void create_nodes(struct mstate const* head, struct list_head* nodes,
                         struct state_node** state_nodes)
{
    *state_nodes = NULL;

    struct mstate const* mm_state = head;
    while (mm_state) {

        struct node* node = malloc(sizeof(struct node));
        node->state = imm_mstate_get_state(mm_state);
        node->mm_state = mm_state;
        node->mark = INITIAL_MARK;
        INIT_LIST_HEAD(&node->edges);
        if (imm_lprob_is_zero(imm_mstate_get_start(node->mm_state)))
            list_add_tail(&node->list_entry, nodes);
        else
            list_add(&node->list_entry, nodes);

        struct state_node* state_node = malloc(sizeof(struct state_node));
        state_node->state = node->state;
        state_node->node = node;
        HASH_ADD_PTR(*state_nodes, state, state_node);

        mm_state = imm_mstate_next_c(mm_state);
    }

    create_edges(nodes, *state_nodes);
}

static void destroy_nodes(struct list_head* nodes)
{
    struct node *node = NULL, *tmp = NULL;

    list_for_each_entry_safe(node, tmp, nodes, list_entry)
    {
        list_del(&node->list_entry);
        destroy_node(node);
    }
}

static void destroy_node(struct node* node)
{
    destroy_edges(&node->edges);
    node->state = NULL;
    node->mm_state = NULL;
    node->idx = -1;
    node->mark = INITIAL_MARK;
    free(node);
}

static void destroy_state_nodes(struct state_node** state_nodes)
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

static void create_edges(struct list_head* nodes, struct state_node* state_nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        struct mm_trans const* trans = imm_mstate_get_trans_c(node->mm_state);
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

static void destroy_edges(struct list_head* edges)
{
    struct edge *edge = NULL, *tmp = NULL;
    list_for_each_entry_safe(edge, tmp, edges, list_entry)
    {
        edge->node = NULL;
        list_del(&edge->list_entry);
        free(edge);
    }
}

static void visit(struct node* node, struct mstate const*** mm_state)
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

static int check_mute_cycles(struct list_head* nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        if (check_mute_visit(node))
            return 1;
    }
    return 0;
}

static int check_mute_visit(struct node* node)
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

static void unmark_nodes(struct list_head* nodes)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry) { node->mark = INITIAL_MARK; }
}
