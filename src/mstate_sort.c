#include "mstate_sort.h"
#include "bug.h"
#include "free.h"
#include "imm/lprob.h"
#include "imm/report.h"
#include "imm/state.h"
#include "khash_ptr.h"
#include "list.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct node
{
    struct imm_state const* state;
    struct mstate const*    mm_state;
    int                     mark;
    struct list_head        edge_list;
    struct list_head        list_entry;
};

struct edge
{
    struct node*     node;
    struct list_head list_entry;
};

KHASH_MAP_INIT_PTR(node, struct node*)

static void create_nodes(struct mstate const** mstates, unsigned nstates,
                         struct list_head* node_list, khash_t(node) * node_table);
static void destroy_node_list(struct list_head* node_list);
static void destroy_node(struct node* node);
static void create_edges(struct list_head* node_list, khash_t(node) * table);
static void destroy_edges(struct list_head* edges);
static void visit(struct node* node, struct mstate const*** mm_state);
static int  check_mute_cycles(struct list_head* node_list);
static int  check_mute_visit(struct node* node);
static void unmark_nodes(struct list_head* node_list);

int mstate_sort(struct mstate const** mstates, unsigned nstates)
{
    struct list_head node_list = LIST_HEAD_INIT(node_list);
    khash_t(node)* node_table = kh_init(node);

    create_nodes(mstates, nstates, &node_list, node_table);

    if (check_mute_cycles(&node_list)) {
        destroy_node_list(&node_list);
        kh_destroy(node, node_table);
        return 1;
    }
    unmark_nodes(&node_list);

    struct mstate const** mstate_arr = malloc(sizeof(struct mstate*) * (size_t)nstates);
    struct mstate const** cur = mstate_arr + nstates;
    struct node*          node = NULL;
    list_for_each_entry(node, &node_list, list_entry) { visit(node, &cur); }

    destroy_node_list(&node_list);
    kh_destroy(node, node_table);

    for (unsigned i = 0; i < nstates; ++i)
        mstates[i] = mstate_arr[i];

    free_c(mstate_arr);
    return 0;
}

static void create_nodes(struct mstate const** mstates, unsigned nstates,
                         struct list_head* node_list, khash_t(node) * node_table)
{
    for (unsigned i = 0; i < nstates; ++i) {
        struct mstate const* mstate = mstates[i];

        struct node* node = malloc(sizeof(struct node));
        node->state = mstate_get_state(mstate);
        node->mm_state = mstate;
        node->mark = INITIAL_MARK;

        INIT_LIST_HEAD(&node->edge_list);
        if (imm_lprob_is_zero(mstate_get_start(node->mm_state)))
            list_add_tail(&node->list_entry, node_list);
        else
            list_add(&node->list_entry, node_list);

        int      ret = 0;
        khiter_t iter = kh_put(node, node_table, node->state, &ret);
        BUG(ret == -1 || ret == 0);
        kh_key(node_table, iter) = node->state;
        kh_val(node_table, iter) = node;
    }

    create_edges(node_list, node_table);
}

static void destroy_node_list(struct list_head* node_list)
{
    struct node *node = NULL, *tmp = NULL;

    list_for_each_entry_safe(node, tmp, node_list, list_entry)
    {
        list_del(&node->list_entry);
        destroy_node(node);
    }
}

static void destroy_node(struct node* node)
{
    destroy_edges(&node->edge_list);
    free_c(node);
}

static void create_edges(struct list_head* node_list, khash_t(node) * table)
{
    struct node* node = NULL;
    list_for_each_entry(node, node_list, list_entry)
    {
        struct mtrans_table const* mtrans_table = mstate_get_mtrans_table(node->mm_state);
        unsigned long              i = 0;
        mtrans_table_for_each(i, mtrans_table)
        {
            if (!mtrans_table_exist(mtrans_table, i))
                continue;
            struct mtrans const* mtrans = mtrans_table_get(mtrans_table, i);
            if (!imm_lprob_is_zero(mtrans_get_lprob(mtrans))) {
                struct edge*            edge = malloc(sizeof(struct edge));
                struct imm_state const* state = mtrans_get_state(mtrans);

                khiter_t iter = kh_get(node, table, state);
                BUG(iter == kh_end(table));

                edge->node = kh_val(table, iter);
                list_add(&edge->list_entry, &node->edge_list);
            }
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
    list_for_each_entry(edge, &node->edge_list, list_entry) { visit(edge->node, mm_state); }
    node->mark = PERMANENT_MARK;
    *mm_state -= 1;
    **mm_state = node->mm_state;
}

static int check_mute_cycles(struct list_head* node_list)
{
    struct node* node = NULL;
    list_for_each_entry(node, node_list, list_entry)
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
    list_for_each_entry(edge, &node->edge_list, list_entry)
    {
        if (check_mute_visit(edge->node))
            return 1;
    }
    node->mark = PERMANENT_MARK;

    return 0;
}

static void unmark_nodes(struct list_head* node_list)
{
    struct node* node = NULL;
    list_for_each_entry(node, node_list, list_entry) { node->mark = INITIAL_MARK; }
}
