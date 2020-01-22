#include "dp.h"
#include "array.h"
#include "free.h"
#include "gmatrix.h"
#include "imm/imm.h"
#include "list.h"
#include "matrix.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "state_idx.h"
#include "string.h"
#include <limits.h>
#include <math.h>

struct trans
{
    struct state_info const* src_state;
    double                   lprob;
};
MAKE_ARRAY_STRUCT(trans, struct trans)
MAKE_ARRAY_INIT(trans)
MAKE_ARRAY_APPEND(trans, struct trans)
MAKE_ARRAY_GET_C(trans, struct trans)
MAKE_ARRAY_LENGTH(trans)
MAKE_ARRAY_EMPTY(trans)

struct state_info
{
    struct imm_state const* state;
    int                     min_seq;
    int                     max_seq;
    double                  start_lprob;
    int                     idx;
    struct array_trans      incoming_transitions;
};

struct step
{
    struct state_info const* state;
    int                      seq_len;
};

struct cell
{
    double      score;
    struct step prev_step;
};
MAKE_GMATRIX_STRUCT(cell, struct cell)
MAKE_GMATRIX_CREATE(cell, struct cell)
MAKE_GMATRIX_GET(cell, struct cell)
MAKE_GMATRIX_GET_C(cell, struct cell)
MAKE_GMATRIX_DESTROY(cell, struct cell)

struct dp_matrix
{
    struct gmatrix_cell* cell;
    int*                 state_col;
};

struct dp
{
    double                   lprob_zero;
    int                      nstates;
    struct state_info*       states;
    struct state_info const* end_state;

    char const* seq;
    int         seq_len;

    struct dp_matrix dp_matrix;
};

static inline int column(struct dp_matrix const* dp_matrix, struct step const* step)
{
    return dp_matrix->state_col[step->state->idx] + step->seq_len - step->state->min_seq;
}

static inline double get_score(struct dp_matrix const* dp_matrix, int row,
                               struct step const* step)
{
    return gmatrix_cell_get_c(dp_matrix->cell, row, column(dp_matrix, step))->score;
}

static double best_trans_score(struct dp const* dp, struct state_info const* dst_state,
                               int const row, struct step* prev_step);
static double final_score(struct dp const* dp, struct step* end_step);
static void   viterbi_path(struct dp* dp, struct imm_path* path, struct step const* end_step);

static void create_trans(struct state_info* states, struct mstate const* const* mm_states,
                         int nstates, struct state_idx* state_idx);

struct dp* imm_dp_create(struct mstate const* const* mm_states, int nstates, char const* seq,
                         struct imm_state const* end_state)
{
    struct dp* dp = malloc(sizeof(struct dp));

    dp->lprob_zero = imm_lprob_zero();
    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)nstates));

    struct state_idx* state_idx = imm_state_idx_create();

    dp->seq = seq;
    dp->seq_len = (int)strlen(seq);

    dp->dp_matrix.state_col = malloc(sizeof(int) * ((size_t)nstates));
    dp->dp_matrix.cell = NULL;
    int next_col = 0;

    for (int i = 0; i < nstates; ++i) {
        dp->states[i].state = imm_mstate_get_state(mm_states[i]);
        dp->states[i].min_seq = imm_state_min_seq(dp->states[i].state);
        dp->states[i].max_seq = imm_state_max_seq(dp->states[i].state);
        dp->states[i].start_lprob = imm_mstate_get_start(mm_states[i]);
        dp->states[i].idx = i;
        array_trans_init(&dp->states[i].incoming_transitions);

        dp->dp_matrix.state_col[i] = next_col;

        imm_state_idx_add(state_idx, dp->states[i].state, i);
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;
    }
    dp->end_state = dp->states + imm_state_idx_find(state_idx, end_state);

    create_trans(dp->states, mm_states, nstates, state_idx);
    imm_state_idx_destroy(state_idx);

    dp->dp_matrix.cell = gmatrix_cell_create(dp->seq_len + 1, next_col);

    return dp;
}

double imm_dp_viterbi(struct dp* dp, struct imm_path* path)
{
    for (int r = 0; r <= dp->seq_len; ++r) {
        char const* seq = dp->seq + r;
        int const   seq_len = dp->seq_len - r;

        struct state_info const* cur = dp->states;

        for (int i = 0; i < dp->nstates; ++i, ++cur) {
            struct step step = {.state = cur};

            for (int len = cur->min_seq; len <= MIN(cur->max_seq, seq_len); ++len) {
                step.seq_len = len;
                int const    col = column(&dp->dp_matrix, &step);
                struct cell* cell = gmatrix_cell_get(dp->dp_matrix.cell, r, col);
                double const score = best_trans_score(dp, cur, r, &cell->prev_step);
                cell->score = score + imm_state_lprob(cur->state, seq, len);
            }
        }
    }

    struct step end_step = {.state = NULL, .seq_len = -1};
    double      score = final_score(dp, &end_step);

    if (path)
        viterbi_path(dp, path, &end_step);

    return score;
}

void imm_dp_destroy(struct dp* dp)
{
    for (int i = 0; i < dp->nstates; ++i)
        array_trans_empty(&dp->states[i].incoming_transitions);

    dp->nstates = -1;

    free_c(dp->states);
    dp->states = NULL;

    dp->end_state = NULL;

    dp->seq = NULL;
    dp->seq_len = -1;

    gmatrix_cell_destroy(dp->dp_matrix.cell);
    dp->dp_matrix.cell = NULL;

    free_c(dp->dp_matrix.state_col);

    free_c(dp);
}

static double best_trans_score(struct dp const* dp, struct state_info const* dst_state,
                               int const row, struct step* prev_step)
{
    double score = dp->lprob_zero;
    prev_step->state = NULL;
    prev_step->seq_len = -1;

    if (row == 0)
        score = dst_state->start_lprob;

    struct array_trans const* incoming = &dst_state->incoming_transitions;

    for (int i = 0; i < array_trans_length(incoming); ++i) {

        struct trans const*      trans = array_trans_get_c(incoming, i);
        struct state_info const* prev = trans->src_state;
        if (row - prev->min_seq < 0)
            continue;

        struct step tmp_step = {.state = prev};
        for (int len = prev->min_seq; len <= MIN(prev->max_seq, row); ++len) {

            if (len == 0 && prev->idx > dst_state->idx)
                continue;

            tmp_step.seq_len = len;
            int    prev_row = row - len;
            double v = get_score(&dp->dp_matrix, prev_row, &tmp_step) + trans->lprob;
            if (v > score) {
                score = v;
                prev_step->state = prev;
                prev_step->seq_len = len;
            }
        }
    }

    if (row > 0 && !prev_step->state)
        return imm_lprob_invalid();
    return score;
}

static double final_score(struct dp const* dp, struct step* end_step)
{
    double                   score = dp->lprob_zero;
    struct state_info const* end_state = dp->end_state;

    end_step->state = NULL;
    end_step->seq_len = -1;
    struct step step = {.state = end_state};

    for (int len = MIN(end_state->max_seq, dp->seq_len); end_state->min_seq <= len; --len) {

        step.seq_len = len;
        double s = get_score(&dp->dp_matrix, dp->seq_len - len, &step);
        if (s > score) {
            score = s;
            end_step->state = dp->end_state;
            end_step->seq_len = len;
        }
    }
    if (!end_step->state)
        return imm_lprob_invalid();
    return score;
}

static void viterbi_path(struct dp* dp, struct imm_path* path, struct step const* end_step)
{
    int                row = dp->seq_len;
    struct step const* step = end_step;

    while (step->seq_len >= 0) {
        imm_path_prepend(path, step->state->state, step->seq_len);
        row -= step->seq_len;
        step = &gmatrix_cell_get_c(dp->dp_matrix.cell, row, column(&dp->dp_matrix, step))
                    ->prev_step;
    }
}

static void create_trans(struct state_info* states, struct mstate const* const* mm_states,
                         int nstates, struct state_idx* state_idx)
{
    for (int i = 0; i < nstates; ++i) {

        struct imm_state const* src_state = imm_mstate_get_state(mm_states[i]);
        int                     src = imm_state_idx_find(state_idx, src_state);

        struct mm_trans const* t = NULL;
        for (t = imm_mstate_get_trans_c(mm_states[i]); t; t = imm_mtrans_next_c(t)) {

            double lprob = imm_mtrans_get_lprob(t);
            if (imm_lprob_is_zero(lprob))
                continue;

            struct imm_state const* dst_state = imm_mtrans_get_state(t);
            int                     dst = imm_state_idx_find(state_idx, dst_state);

            struct trans* trans = array_trans_append(&states[dst].incoming_transitions);
            trans->lprob = lprob;
            trans->src_state = states + src;
        }
    }
}
