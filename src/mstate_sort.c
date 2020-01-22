#include "mstate_sort.h"
#include "free.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "list.h"
#include "mstate.h"
#include "mtrans.h"
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
};

KHASH_MAP_INIT_PTR(state_node, struct state_node*)

static void create_nodes(struct mstate const** mstates, int nstates, struct list_head* nodes,
                         khash_t(state_node) * table);
static void destroy_nodes(struct list_head* nodes);
static void destroy_node(struct node* node);
static void destroy_state_nodes(khash_t(state_node) * table);
static void create_edges(struct list_head* nodes, khash_t(state_node) * table);
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
    khash_t(state_node)* tbl = kh_init(state_node);
    create_nodes(mstates, nstates, &nodes, tbl);

    if (check_mute_cycles(&nodes)) {
        destroy_nodes(&nodes);
        destroy_state_nodes(tbl);
        return 1;
    }
    unmark_nodes(&nodes);

    struct mstate const** mm_state = malloc(sizeof(struct mstate*) * (size_t)nstates);
    struct mstate const** cur = mm_state + nstates;
    struct node*          node = NULL;
    list_for_each_entry(node, &nodes, list_entry) { visit(node, &cur); }

    destroy_nodes(&nodes);
    destroy_state_nodes(tbl);

    for (int i = 0; i < nstates; ++i)
        mstates[i] = mm_state[i];

    free_c(mm_state);
    return 0;
}

static void create_nodes(struct mstate const** mstates, int nstates, struct list_head* nodes,
                         khash_t(state_node) * table)
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

        struct state_node* state_node = malloc(sizeof(struct state_node));
        state_node->state = node->state;
        state_node->node = node;

        int      ret = 0;
        khiter_t iter = kh_put(state_node, table, state_node->state, &ret);
        if (ret == -1)
            imm_die("hash table failed");
        if (ret == 0)
            imm_die("state already exist");

        kh_key(table, iter) = state_node->state;
        kh_val(table, iter) = state_node;
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
    node->state = NULL;
    node->mm_state = NULL;
    node->idx = -1;
    node->mark = INITIAL_MARK;
    free_c(node);
}

static void destroy_state_nodes(khash_t(state_node) * table)
{
    for (khiter_t i = kh_begin(table); i < kh_end(table); ++i) {
        if (kh_exist(table, i))
            free_c(kh_val(table, i));
    }

    kh_destroy(state_node, table);
}

static void create_edges(struct list_head* nodes, khash_t(state_node) * table)
{
    struct node* node = NULL;
    list_for_each_entry(node, nodes, list_entry)
    {
        struct mm_trans const* trans = mstate_get_trans_c(node->mm_state);
        while (trans) {
            if (!imm_lprob_is_zero(mtrans_get_lprob(trans))) {
                struct edge*            edge = malloc(sizeof(struct edge));
                struct imm_state const* state = mtrans_get_state(trans);

                khiter_t i = kh_get(state_node, table, state);
                if (i == kh_end(table))
                    imm_die("state not found");

                edge->node = kh_val(table, i)->node;
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
        edge->node = NULL;
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
