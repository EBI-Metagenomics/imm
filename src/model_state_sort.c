#include "model_state_sort.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "list.h"
#include "log.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "std.h"

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct node
{
    struct imm_state const*   state;
    struct model_state const* mstate;
    int                       mark;
    struct list_head          edge_list;
    struct list_head          list_entry;
};

struct edge
{
    struct node*     node;
    struct list_head list_entry;
};

KHASH_MAP_INIT_PTR(node, struct node*)

static int        check_mute_cycles(struct list_head* node_list);
static int        check_mute_visit(struct node* node);
static void       create_edges(struct list_head* node_list, khash_t(node) * table);
static void       create_nodes(struct model_state const** mstates, uint32_t nstates, struct list_head* node_list,
                               khash_t(node) * node_table);
static void       destroy_edges(struct list_head* edges);
static void       destroy_node(struct node* node);
static void       destroy_node_list(struct list_head* node_list);
static inline int name_compare(void const* a, void const* b);
static void       unmark_nodes(struct list_head* node_list);
static void       visit(struct node* node, struct model_state const*** mstate);

void model_state_name_sort(struct model_state const** mstates, uint32_t nstates)
{
    qsort(mstates, nstates, sizeof(*mstates), name_compare);
}

int model_state_topological_sort(struct model_state const** mstates, uint32_t nstates)
{
    struct list_head node_list = LIST_HEAD_INIT(node_list);
    khash_t(node)* node_table = kh_init(node);

    create_nodes(mstates, nstates, &node_list, node_table);

    if (check_mute_cycles(&node_list)) {
        error("mute cycles are not allowed");
        destroy_node_list(&node_list);
        kh_destroy(node, node_table);
        return 1;
    }
    unmark_nodes(&node_list);

    struct model_state const** mstate_arr = malloc(sizeof(*mstate_arr) * nstates);
    struct model_state const** cur = mstate_arr + nstates;
    struct node*               node = NULL;
    list_for_each_entry (node, &node_list, list_entry) {
        visit(node, &cur);
    }

    destroy_node_list(&node_list);
    kh_destroy(node, node_table);

    for (uint32_t i = 0; i < nstates; ++i)
        mstates[i] = mstate_arr[i];

    free(mstate_arr);
    return 0;
}

static int check_mute_cycles(struct list_head* node_list)
{
    struct node* node = NULL;
    list_for_each_entry (node, node_list, list_entry) {
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
        return 1;
    }

    node->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    list_for_each_entry (edge, &node->edge_list, list_entry) {
        if (check_mute_visit(edge->node))
            return 1;
    }
    node->mark = PERMANENT_MARK;

    return 0;
}

static void create_edges(struct list_head* node_list, khash_t(node) * table)
{
    struct node* node = NULL;
    list_for_each_entry (node, node_list, list_entry) {

        struct model_trans_table const* mtrans_table = model_state_get_mtrans_table(node->mstate);
        struct model_trans const**      mtrans = model_trans_table_array(mtrans_table);

        for (uint32_t i = 0; i < model_trans_table_size(mtrans_table); ++i) {

            struct model_trans const* t = mtrans[i];

            struct edge* edge = malloc(sizeof(*edge));

            khiter_t iter = kh_get(node, table, model_trans_get_state(t));
            BUG(iter == kh_end(table));

            edge->node = kh_val(table, iter);
            list_add(&edge->list_entry, &node->edge_list);
        }
        free(mtrans);
    }
}

static void create_nodes(struct model_state const** mstates, uint32_t nstates, struct list_head* node_list,
                         khash_t(node) * node_table)
{
    for (uint32_t i = 0; i < nstates; ++i) {
        struct model_state const* mstate = mstates[i];

        struct node* node = malloc(sizeof(*node));
        node->state = model_state_get_state(mstate);
        node->mstate = mstate;
        node->mark = INITIAL_MARK;

        INIT_LIST_HEAD(&node->edge_list);
        if (imm_lprob_is_zero(model_state_get_start(node->mstate)))
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

static void destroy_edges(struct list_head* edges)
{
    struct edge *edge = NULL, *tmp = NULL;
    list_for_each_entry_safe (edge, tmp, edges, list_entry) {
        list_del(&edge->list_entry);
        free(edge);
    }
}

static void destroy_node(struct node* node)
{
    destroy_edges(&node->edge_list);
    free(node);
}

static void destroy_node_list(struct list_head* node_list)
{
    struct node *node = NULL, *tmp = NULL;

    list_for_each_entry_safe (node, tmp, node_list, list_entry) {
        list_del(&node->list_entry);
        destroy_node(node);
    }
}

static inline int name_compare(void const* a, void const* b)
{
    char const* left = imm_state_name((*(struct model_state const**)a)->state);
    char const* right = imm_state_name((*(struct model_state const**)b)->state);
    return strcmp(left, right);
}

static void unmark_nodes(struct list_head* node_list)
{
    struct node* node = NULL;
    list_for_each_entry (node, node_list, list_entry) {
        node->mark = INITIAL_MARK;
    }
}

static void visit(struct node* node, struct model_state const*** mstate)
{
    if (node->mark == PERMANENT_MARK)
        return;
    if (node->mark == TEMPORARY_MARK)
        return;

    node->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    list_for_each_entry (edge, &node->edge_list, list_entry) {
        visit(edge->node, mstate);
    }
    node->mark = PERMANENT_MARK;
    *mstate -= 1;
    **mstate = node->mstate;
}
