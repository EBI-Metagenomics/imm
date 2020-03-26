#include "dp2.h"
#include "dp2_emission.h"
#include "dp2_matrix.h"
#include "dp2_states.h"
#include "dp2_step.h"
#include "dp2_trans.h"
#include "free.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/report.h"
#include "imm/seq_code.h"
#include "imm/state.h"
#include "imm/step.h"
#include "imm/subseq.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "state_idx.h"
#include <limits.h>
#include <string.h>

static double best_trans_cost(struct dp2 const* dp, struct dp2_matrix const* matrix,
                              unsigned target_state, unsigned row, struct dp2_step* prev_step);
static double final_score2(struct dp2 const* dp, struct dp2_matrix const* matrix,
                           struct dp2_step* end_step);
#if 0
static void   viterbi_path(struct dp const* dp, struct dp_matrix const* matrix,
                           struct imm_path* path, struct step const* end_step);

static void create_reversed_trans(struct state_info* states, struct mstate const* const* mstates,
                         unsigned nstates, struct state_idx* state_idx);
#endif

struct dp2
{
    struct seq_code const*     seq_code;
    struct dp2_emission const* emission;
    struct dp2_trans const*    transition;
    struct dp2_states const*   states;
};

static unsigned min_seq(struct mstate const* const* mstates, unsigned nstates);
static unsigned max_seq(struct mstate const* const* mstates, unsigned nstates);

struct dp2 const* dp2_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                             unsigned const nstates, struct imm_state const* end_state)
{
    struct dp2* dp = malloc(sizeof(struct dp2));
    dp->seq_code =
        imm_seq_code_create(abc, min_seq(mstates, nstates), max_seq(mstates, nstates));

    struct state_idx* state_idx = state_idx_create();
    dp->states = dp2_states_create(mstates, nstates, end_state, state_idx);
    dp->emission = dp2_emission_create(dp->seq_code, mstates, nstates);
    dp->transition = dp2_trans_create(mstates, nstates, state_idx);
    state_idx_destroy(state_idx);

    return dp;
}

struct dp2_states const* dp2_states(struct dp2 const* dp) { return dp->states; }

double dp2_viterbi(struct dp2 const* dp, struct dp2_matrix* matrix)
{
    struct imm_seq const* seq = dp2_matrix_get_seq(matrix);

    for (unsigned r = 0; r <= imm_seq_length(seq); ++r) {
        IMM_BUG(imm_seq_length(seq) < r);
        unsigned const seq_len = imm_seq_length(seq) - r;

        for (unsigned i = 0; i < dp2_states_nstates(dp->states); ++i) {

            unsigned const min_len = dp2_states_min_seq(dp->states, i);
            unsigned const max_len = MIN(dp2_states_max_seq(dp->states, i), seq_len);

            struct dp2_step* prev_step = dp2_matrix_get_prev_step(matrix, r, i);
            double const     score = best_trans_cost(dp, matrix, i, r, prev_step);

            for (unsigned len = min_len; len <= max_len; ++len) {
                struct dp2_step const step = {.state = i, .seq_len = len};
                /* TODO: I think we dont need to have a prev_step
                 * for each cell of the cost matrix*/
                IMM_SUBSEQ(subseq, dp2_matrix_get_seq(matrix), r, len);
                unsigned seq_code =
                    imm_seq_code_encode(dp->seq_code, min_len, imm_subseq_cast(&subseq));
                double v = dp2_emission_cost(dp->emission, i, seq_code);
                double cost = score + v;
                dp2_matrix_set_cost(matrix, r, &step, cost);
            }
        }
    }

    struct dp2_step end_step = {.state = UINT_MAX, .seq_len = UINT_MAX};
    double          fs = final_score2(dp, matrix, &end_step);
    return fs;
}

void dp2_destroy(struct dp2 const* dp)
{
    imm_seq_code_destroy(dp->seq_code);
    dp2_emission_destroy(dp->emission);
    dp2_trans_destroy(dp->transition);
    dp2_states_destroy(dp->states);
}

static double best_trans_cost(struct dp2 const* dp, struct dp2_matrix const* matrix,
                              unsigned target_state, unsigned row, struct dp2_step* prev_step)
{
    double score = imm_lprob_zero();
    prev_step->state = UINT_MAX;
    prev_step->seq_len = UINT_MAX;

    if (row == 0)
        score = dp2_states_start_lprob(dp->states, target_state);

    for (unsigned i = 0; i < dp2_trans_ntrans(dp->transition, target_state); ++i) {

        unsigned source_state = dp2_trans_source_state(dp->transition, target_state, i);
        if (row < dp2_states_min_seq(dp->states, source_state))
            continue;

        for (unsigned len = dp2_states_min_seq(dp->states, source_state);
             len <= MIN(dp2_states_max_seq(dp->states, source_state), row); ++len) {

            if (len == 0 && source_state > target_state)
                continue;

            struct dp2_step step = {.state = source_state, .seq_len = len};

            IMM_BUG(row < len);
            unsigned const prev_row = row - len;

            double v0 = dp2_matrix_get_cost(matrix, prev_row, &step);
            double v1 = dp2_trans_cost(dp->transition, target_state, i);
            double v = v0 + v1;

            if (v > score) {
                score = v;
                prev_step->state = source_state;
                prev_step->seq_len = len;
            }
        }
    }

#if 0
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
            IMM_BUG(row < len);
            unsigned const prev_row = row - len;
            double const   v = get_score(matrix, prev_row, &tmp_step) + trans->lprob;
            if (v > score) {
                score = v;
                prev_step->state = prev;
                prev_step->seq_len = len;
            }
        }
    }

#endif
    if (row > 0 && prev_step->state == UINT_MAX)
        return imm_lprob_invalid();

    return score;
}

static double final_score2(struct dp2 const* dp, struct dp2_matrix const* matrix,
                           struct dp2_step* end_step)
{
    double   score = imm_lprob_zero();
    unsigned end_state = dp2_states_end_state(dp->states);

    end_step->state = UINT_MAX;
    end_step->seq_len = UINT_MAX;
    struct dp2_step step = {.state = end_state};

    struct imm_seq const* seq = dp2_matrix_get_seq(matrix);

    for (unsigned len = MIN(dp2_states_max_seq(dp->states, end_state), imm_seq_length(seq));;
         --len) {

        step.seq_len = len;
        double s = dp2_matrix_get_cost(matrix, imm_seq_length(seq) - len, &step);
        if (s > score) {
            score = s;
            end_step->state = end_state;
            end_step->seq_len = len;
        }

        if (dp2_states_min_seq(dp->states, end_state) == len)
            break;
    }
    if (end_step->state == UINT_MAX)
        return imm_lprob_invalid();

    return score;
}

#if 0
static void viterbi_path(struct dp const* dp, struct dp_matrix const* matrix,
                         struct imm_path* path, struct step const* end_step)
{
    unsigned           row = imm_seq_length(matrix->seq);
    struct step const* step = end_step;

    while (step->seq_len != UINT_MAX) {
        struct imm_step* new_step = imm_step_create(step->state->state, step->seq_len);
        IMM_BUG(new_step == NULL);
        imm_path_prepend(path, new_step);
        IMM_BUG(step->seq_len > row);
        row -= step->seq_len;
        step = &matrix_cell_get_c(matrix->cell, row, column(matrix, step))->prev_step;
    }
}
#endif

static unsigned min_seq(struct mstate const* const* mstates, unsigned nstates)
{
    IMM_BUG(nstates == 0);

    unsigned min = imm_state_min_seq(mstate_get_state(mstates[0]));
    for (unsigned i = 1; i < nstates; ++i)
        min = MIN(min, imm_state_min_seq(mstate_get_state(mstates[i])));

    return min;
}

static unsigned max_seq(struct mstate const* const* mstates, unsigned nstates)
{
    IMM_BUG(nstates == 0);

    unsigned max = imm_state_max_seq(mstate_get_state(mstates[0]));
    for (unsigned i = 1; i < nstates; ++i)
        max = MAX(max, imm_state_max_seq(mstate_get_state(mstates[i])));

    return max;
}
