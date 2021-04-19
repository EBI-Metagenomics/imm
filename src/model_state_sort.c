#include "model_state_sort.h"
#include "containers/containers.h"
#include "imm/imm.h"
#include "khash_ptr.h"
#include "log.h"
#include "model_state.h"
#include "model_trans.h"
#include "model_trans_table.h"
#include "std.h"

#define INITIAL_MARK 0
#define TEMPORARY_MARK 1
#define PERMANENT_MARK 2

struct vert
{
    struct imm_state const*   state;
    struct model_state const* mstate;
    int                       mark;
    struct queue              edgeq;
    struct node               node;
};

struct edge
{
    struct vert* vert;
    struct node  node;
};

KHASH_MAP_INIT_PTR(node, struct vert*)

static int        check_mute_cycles(struct queue* node_list);
static int        check_mute_visit(struct vert* vert);
static void       create_edges(struct queue* node_list, khash_t(node) * table);
static void       create_nodes(struct model_state const** mstates, uint16_t nstates, struct queue* vertq,
                               khash_t(node) * node_table);
static void       destroy_edges(struct queue* edges);
static void       destroy_node(struct vert* vert);
static void       destroy_node_list(struct queue* node_list);
static inline int name_compare(void const* a, void const* b);
static void       unmark_nodes(struct queue* node_list);
static void       visit(struct vert* vert, struct model_state const*** mstate);

void model_state_name_sort(struct model_state const** mstates, uint16_t nstates)
{
    qsort(mstates, nstates, sizeof(*mstates), name_compare);
}

int model_state_topological_sort(struct model_state const** mstates, uint16_t nstates)
{
    struct queue vertq = QUEUE_INIT(vertq);
    khash_t(node)* vert_tbl = kh_init(node);

    create_nodes(mstates, nstates, &vertq, vert_tbl);

    if (check_mute_cycles(&vertq)) {
        error("mute cycles are not allowed");
        destroy_node_list(&vertq);
        kh_destroy(node, vert_tbl);
        return 1;
    }
    unmark_nodes(&vertq);

    struct model_state const** mstate_arr = malloc(sizeof(*mstate_arr) * nstates);
    struct model_state const** cur = mstate_arr + nstates;
    struct vert*               vert = NULL;
    struct iter                iter = queue_iter(&vertq);
    iter_for_each_entry(vert, &iter, node) { visit(vert, &cur); }

    destroy_node_list(&vertq);
    kh_destroy(node, vert_tbl);

    for (uint16_t i = 0; i < nstates; ++i)
        mstates[i] = mstate_arr[i];

    free(mstate_arr);
    return 0;
}

static int check_mute_cycles(struct queue* node_list)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(node_list);
    iter_for_each_entry(vert, &iter, node)
    {
        if (check_mute_visit(vert))
            return 1;
    }
    return 0;
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

static void create_edges(struct queue* node_list, khash_t(node) * table)
{
    struct vert* node = NULL;
    struct iter  iter = queue_iter(node_list);
    iter_for_each_entry(node, &iter, node)
    {

        struct model_trans_table const* mtrans_table = model_state_get_mtrans_table(node->mstate);
        struct model_trans const**      mtrans = model_trans_table_array(mtrans_table);

        for (uint16_t i = 0; i < model_trans_table_size(mtrans_table); ++i) {

            struct model_trans const* t = mtrans[i];

            struct edge* edge = malloc(sizeof(*edge));

            khiter_t it = kh_get(node, table, model_trans_get_state(t));
            BUG(it == kh_end(table));

            edge->vert = kh_val(table, it);
            queue_put(&node->edgeq, &edge->node);
        }
        free(mtrans);
    }
}

static void create_nodes(struct model_state const** mstates, uint16_t nstates, struct queue* vertq,
                         khash_t(node) * node_table)
{
    for (uint16_t i = 0; i < nstates; ++i) {
        struct model_state const* mstate = mstates[i];

        struct vert* node = malloc(sizeof(*node));
        node->state = model_state_get_state(mstate);
        node->mstate = mstate;
        node->mark = INITIAL_MARK;

        queue_init(&node->edgeq);
        if (imm_lprob_is_zero(model_state_get_start(node->mstate)))
            queue_put(vertq, &node->node);
        else
            queue_put_first(vertq, &node->node);

        int      ret = 0;
        khiter_t iter = kh_put(node, node_table, node->state, &ret);
        BUG(ret == -1 || ret == 0);
        kh_key(node_table, iter) = node->state;
        kh_val(node_table, iter) = node;
    }

    create_edges(vertq, node_table);
}

static void destroy_edges(struct queue* edges) { queue_init(edges); }

static void destroy_node(struct vert* vert) { destroy_edges(&vert->edgeq); }

static void destroy_node_list(struct queue* node_list) { queue_init(node_list); }

static inline int name_compare(void const* a, void const* b)
{
    char const* left = imm_state_name((*(struct model_state const**)a)->state);
    char const* right = imm_state_name((*(struct model_state const**)b)->state);
    return strcmp(left, right);
}

static void unmark_nodes(struct queue* node_list)
{
    struct vert* vert = NULL;
    struct iter  iter = queue_iter(node_list);
    iter_for_each_entry(vert, &iter, node) { vert->mark = INITIAL_MARK; }
}

static void visit(struct vert* vert, struct model_state const*** mstate)
{
    if (vert->mark == PERMANENT_MARK)
        return;
    if (vert->mark == TEMPORARY_MARK)
        return;

    vert->mark = TEMPORARY_MARK;
    struct edge const* edge = NULL;
    struct iter        iter = queue_iter(&vert->edgeq);
    iter_for_each_entry(edge, &iter, node) { visit(edge->vert, mstate); }
    vert->mark = PERMANENT_MARK;
    *mstate -= 1;
    **mstate = vert->mstate;
}
