#include "mstate_sort.h"
#include "bug.h"
#include "free.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "list.h"
#include "mstate.h"
#include "mtrans.h"
#include "state_idx.h"
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
    int                     mark;
    struct list_head        edges;
    struct list_head        list_entry;
};

struct edge
{
    struct node*     node;
    struct list_head list_entry;
};

KHASH_MAP_INIT_PTR(node, struct node*)

static void create_nodes(struct mstate const** mstates, int nstates, struct list_head* nodes,
                         khash_t(node) * table);
static void destroy_nodes(struct list_head* nodes);
static void destroy_node(struct node* node);
static void create_edges(struct list_head* nodes, khash_t(node) * table);
static void destroy_edges(struct list_head* edges);
static void visit(struct node* node, struct mstate const*** mm_state);
static int  check_mute_cycles(struct list_head* nodes);
static int  check_mute_visit(struct node* node);
static void unmark_nodes(struct list_head* nodes);

int mstate_sort(struct mstate const** mstates, int nstates)
{
    struct list_head nodes = LIST_HEAD_INIT(nodes);

    /* TODO: i dont think i need a hash table for that if we start
     * receiving an array of mstates.*/
    khash_t(node)* tbl = kh_init(node);
    create_nodes(mstates, nstates, &nodes, tbl);

    if (check_mute_cycles(&nodes)) {
        destroy_nodes(&nodes);
        kh_destroy(node, tbl);
        return 1;
    }
    unmark_nodes(&nodes);

    struct mstate const** mm_state = malloc(sizeof(struct mstate*) * (size_t)nstates);
    struct mstate const** cur = mm_state + nstates;
    struct node*          node = NULL;
    list_for_each_entry(node, &nodes, list_entry) { visit(node, &cur); }

    destroy_nodes(&nodes);
    kh_destroy(node, tbl);

    for (int i = 0; i < nstates; ++i)
        mstates[i] = mm_state[i];

    free_c(mm_state);
    return 0;
}

static void create_nodes(struct mstate const** mstates, int nstates, struct list_head* nodes,
                         khash_t(node) * table)
{
    for (int i = 0; i < nstates; ++i) {
        struct mstate const* mstate = mstates[i];

        struct node* node = malloc(sizeof(struct node));
        node->state = mstate_get_state(mstate);
        node->mm_state = mstate;
        node->mark = INITIAL_MARK;
        INIT_LIST_HEAD(&node->edges);
        if (imm_lprob_is_zero(mstate_get_start(node->mm_state)))
            list_add_tail(&node->list_entry, nodes);
        else
            list_add(&node->list_entry, nodes);

        int      ret = 0;
        khiter_t iter = kh_put(node, table, node->state, &ret);
        BUG(ret == -1 || ret == 0);
        kh_key(table, iter) = node->state;
        kh_val(table, iter) = node;
    }

    create_edges(nodes, table);
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
    free_c(node);
}

static void create_edges(struct list_head* nodes, khash_t(node) * table)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        struct mm_trans const* trans = mstate_get_trans_c(node->mm_state);
        while (trans) {
            if (!imm_lprob_is_zero(mtrans_get_lprob(trans))) {
                struct edge*            edge = malloc(sizeof(struct edge));
                struct imm_state const* state = mtrans_get_state(trans);

                khiter_t i = kh_get(node, table, state);
                BUG(i == kh_end(table));

                edge->node = kh_val(table, i);
                list_add(&edge->list_entry, &node->edges);
            }

            trans = mtrans_next_c(trans);
        }
    }
}

static void destroy_edges(struct list_head* edges)
{
    struct edge *edge = NULL, *tmp = NULL;
    list_for_each_entry_safe(edge, tmp, edges, list_entry)
    {
        list_del(&edge->list_entry);
        free_c(edge);
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
