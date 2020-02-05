#include "dp.h"
#include "dp_matrix.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/report.h"
#include "imm/state.h"
#include "imm/subseq.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state_idx.h"
#include <limits.h>
#include <string.h>

static double best_trans_score(struct dp const* dp, struct dp_matrix const* matrix,
                               struct state_info const* dst_state, unsigned const row,
                               struct step* prev_step);
static double final_score(struct dp const* dp, struct dp_matrix const* matrix,
                          struct step* end_step);
static void   viterbi_path(struct dp const* dp, struct dp_matrix const* matrix,
                           struct imm_path* path, struct step const* end_step);

static void create_trans(struct state_info* states, struct mstate const* const* mstates,
                         unsigned nstates, struct state_idx* state_idx);

struct dp const* dp_create(struct mstate const* const* mstates, unsigned const nstates,
                           struct imm_seq const* seq, struct imm_state const* end_state)
{
    struct dp* dp = malloc(sizeof(struct dp));

    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)nstates));

    struct state_idx* state_idx = state_idx_create();

    unsigned next_col = 0;

    for (unsigned i = 0; i < nstates; ++i) {
        dp->states[i].state = mstate_get_state(mstates[i]);
        dp->states[i].min_seq = imm_state_min_seq(dp->states[i].state);
        dp->states[i].max_seq = imm_state_max_seq(dp->states[i].state);
        dp->states[i].start_lprob = mstate_get_start(mstates[i]);
        dp->states[i].idx = i;
        array_trans_init(&dp->states[i].incoming_transitions);

        state_idx_add(state_idx, dp->states[i].state, i);
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;
    }
    dp->end_state = dp->states + state_idx_find(state_idx, end_state);

    create_trans(dp->states, mstates, nstates, state_idx);
    state_idx_destroy(state_idx);
    dp->mstates = mstates;

    return dp;
}

double dp_viterbi(struct dp const* dp, struct dp_matrix* matrix, struct imm_path* path)
{
    for (unsigned r = 0; r <= imm_seq_length(matrix->seq); ++r) {
        BUG(imm_seq_length(matrix->seq) < r);
        unsigned const seq_len = imm_seq_length(matrix->seq) - r;

        struct state_info const* cur = dp->states;

        for (unsigned i = 0; i < dp->nstates; ++i, ++cur) {
            struct step step = {.state = cur};

            for (unsigned len = cur->min_seq; len <= MIN(cur->max_seq, seq_len); ++len) {
                step.seq_len = len;
                unsigned const col = column(matrix, &step);
                struct cell*   cell = matrix_cell_get(matrix->cell, r, col);
                double const   score = best_trans_score(dp, matrix, cur, r, &cell->prev_step);
                IMM_SUBSEQ(subseq, matrix->seq, r, len);
                cell->score = score + imm_state_lprob(cur->state, imm_subseq_cast(&subseq));
            }
        }
    }

    struct step end_step = {.state = NULL, .seq_len = UINT_MAX};
    double      score = final_score(dp, matrix, &end_step);

    if (path)
        viterbi_path(dp, matrix, path, &end_step);

    return score;
}

void dp_destroy(struct dp const* dp)
{
    for (unsigned i = 0; i < dp->nstates; ++i)
        array_trans_empty(&dp->states[i].incoming_transitions);

    free_c(dp->states);
    free_c(dp->mstates);
    free_c(dp);
}

static double best_trans_score(struct dp const* dp, struct dp_matrix const* matrix,
                               struct state_info const* dst_state, unsigned const row,
                               struct step* prev_step)
{
    double score = IMM_LPROB_ZERO;
    prev_step->state = NULL;
    prev_step->seq_len = UINT_MAX;

    if (row == 0)
        score = dst_state->start_lprob;

    struct array_trans const* incoming = &dst_state->incoming_transitions;

    for (unsigned i = 0; i < array_trans_length(incoming); ++i) {

        struct trans const*      trans = array_trans_get_c(incoming, i);
        struct state_info const* prev = trans->src_state;
        if (row < prev->min_seq)
            continue;

        struct step tmp_step = {.state = prev};
        for (unsigned len = prev->min_seq; len <= MIN(prev->max_seq, row); ++len) {

            if (len == 0 && prev->idx > dst_state->idx)
                continue;

            tmp_step.seq_len = len;
            BUG(row < len);
            unsigned const prev_row = row - len;
            double const   v = get_score(matrix, prev_row, &tmp_step) + trans->lprob;
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

static double final_score(struct dp const* dp, struct dp_matrix const* matrix,
                          struct step* end_step)
{
    double                   score = IMM_LPROB_ZERO;
    struct state_info const* end_state = dp->end_state;

    end_step->state = NULL;
    end_step->seq_len = UINT_MAX;
    struct step step = {.state = end_state};

    for (unsigned len = MIN(end_state->max_seq, imm_seq_length(matrix->seq));; --len) {

        step.seq_len = len;
        double s = get_score(matrix, imm_seq_length(matrix->seq) - len, &step);
        if (s > score) {
            score = s;
            end_step->state = dp->end_state;
            end_step->seq_len = len;
        }

        if (end_state->min_seq == len)
            break;
    }
    if (!end_step->state)
        return imm_lprob_invalid();
    return score;
}

static void viterbi_path(struct dp const* dp, struct dp_matrix const* matrix,
                         struct imm_path* path, struct step const* end_step)
{
    unsigned           row = imm_seq_length(matrix->seq);
    struct step const* step = end_step;

    while (step->seq_len != UINT_MAX) {
        imm_path_prepend(path, step->state->state, step->seq_len);
        BUG(step->seq_len > row);
        row -= step->seq_len;
        step = &matrix_cell_get_c(matrix->cell, row, column(matrix, step))->prev_step;
    }
}

static void create_trans(struct state_info* states, struct mstate const* const* mstates,
                         unsigned nstates, struct state_idx* state_idx)
{
    for (unsigned i = 0; i < nstates; ++i) {

        struct imm_state const*    src_state = mstate_get_state(mstates[i]);
        unsigned                   src = state_idx_find(state_idx, src_state);
        struct mtrans_table const* table = mstate_get_mtrans_table(mstates[i]);

        unsigned long iter = 0;
        mtrans_table_for_each (iter, table) {
            if (!mtrans_table_exist(table, iter))
                continue;
            struct mtrans const* mtrans = mtrans_table_get(table, iter);
            double               lprob = mtrans_get_lprob(mtrans);
            if (imm_lprob_is_zero(lprob))
                continue;

            struct imm_state const* dst_state = mtrans_get_state(mtrans);
            unsigned                dst = state_idx_find(state_idx, dst_state);

            struct trans* trans = array_trans_append(&states[dst].incoming_transitions);
            trans->lprob = lprob;
            trans->src_state = states + src;
        }
    }
}
