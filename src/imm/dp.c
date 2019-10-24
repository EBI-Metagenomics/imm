#include "src/imm/dp.h"
#include "imm.h"
#include "src/imm/gmatrix.h"
#include "src/imm/hide.h"
#include "src/imm/matrix.h"
#include "src/imm/min.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_trans.h"
#include "src/imm/state_idx.h"
#include "src/uthash/uthash.h"
#include <limits.h>
#include <math.h>

struct state_info
{
    const struct imm_state* state;
    double                  start_lprob;
    int                     idx;
};

static inline int min_seq(struct state_info const* state_info)
{
    return imm_state_min_seq(state_info->state);
}
static inline int max_seq(struct state_info const* state_info)
{
    return imm_state_max_seq(state_info->state);
}

struct step
{
    struct state_info const* state;
    int                      seq_len;
};
MAKE_GMATRIX_STRUCT(step, struct step)
MAKE_GMATRIX_CREATE(step, struct step)
MAKE_GMATRIX_GET(step, struct step)
MAKE_GMATRIX_DESTROY(step, struct step)

struct dp_matrix
{
    struct matrix*       score;
    struct gmatrix_step* step;
    int*                 state_col;
};

struct dp
{
    int                      nstates;
    struct state_info*       states;
    struct state_info const* end_state;

    const char* seq;
    int         seq_len;

    struct matrix* trans;

    struct dp_matrix dp_matrix;
};

HIDE int    column(struct dp_matrix const* dp_matrix, struct step const* step);
HIDE double get_score(struct dp_matrix const* dp_matrix, int row, struct step const* step);
HIDE void   set_score(struct dp_matrix const* dp_matrix, int row, struct step const* step,
                      double score);

HIDE double best_trans_score(const struct dp* dp, struct state_info const* dst_state, int row,
                             struct step* prev_step);
HIDE double final_score(struct dp const* dp, struct step* end_step);
HIDE int    viterbi_path(struct dp* dp, struct imm_path* path, struct step const* end_step);

HIDE struct matrix* create_trans(const struct mm_state* const* mm_states, int nstates,
                                 const struct state_idx* state_idx);

struct dp* dp_create(const struct mm_state* const* mm_states, int nstates, const char* seq,
                     struct imm_state const* end_state)
{
    struct dp* dp = malloc(sizeof(struct dp));

    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)nstates));

    struct state_idx* state_idx = NULL;
    state_idx_create(&state_idx);

    dp->seq = seq;
    dp->seq_len = (int)strlen(seq);

    dp->dp_matrix.state_col = malloc(sizeof(int) * ((size_t)nstates));
    dp->dp_matrix.score = NULL;
    dp->dp_matrix.step = NULL;
    int next_col = 0;

    for (int i = 0; i < nstates; ++i) {
        dp->states[i].state = mm_state_get_state(mm_states[i]);
        dp->states[i].start_lprob = mm_state_get_start_lprob(mm_states[i]);
        dp->states[i].idx = i;

        dp->dp_matrix.state_col[i] = next_col;

        state_idx_add(&state_idx, dp->states[i].state, i);
        next_col += imm_state_max_seq(dp->states[i].state) -
                    imm_state_min_seq(dp->states[i].state) + 1;
    }
    dp->end_state = dp->states + state_idx_find(state_idx, end_state);

    dp->trans = create_trans(mm_states, nstates, state_idx);
    state_idx_destroy(&state_idx);

    dp->dp_matrix.score = matrix_create(dp->seq_len + 1, next_col);
    dp->dp_matrix.step = gmatrix_step_create(dp->seq_len + 1, next_col);

    return dp;
}

double dp_viterbi(struct dp* dp, struct imm_path* path)
{
    for (int r = 0; r <= dp->seq_len; ++r) {
        char const* seq = dp->seq + r;
        int         seq_len = dp->seq_len - r;

        struct state_info const* cur = dp->states;
        for (int i = 0; i < dp->nstates; ++i, ++cur) {

            for (int len = imm_state_min_seq(cur->state);
                 len <= MIN(imm_state_max_seq(cur->state), seq_len); ++len) {

                struct step  cur_step = {cur, len};
                int          col = column(&dp->dp_matrix, &cur_step);
                struct step* prev_step = gmatrix_step_get(dp->dp_matrix.step, r, col);
                double       score = best_trans_score(dp, cur, r, prev_step);
                double       emiss = imm_state_lprob(cur->state, seq, len);
                set_score(&dp->dp_matrix, r, &cur_step, score + emiss);
            }
        }
    }

    struct step end_step = {NULL, -1};
    double      score = final_score(dp, &end_step);

    if (path) {
        if (viterbi_path(dp, path, &end_step))
            return NAN;
    }

    return score;
}

void dp_destroy(struct dp* dp)
{
    dp->nstates = -1;

    free(dp->states);
    dp->states = NULL;

    dp->end_state = NULL;

    dp->seq = NULL;
    dp->seq_len = -1;

    matrix_destroy(dp->trans);
    dp->trans = NULL;

    matrix_destroy(dp->dp_matrix.score);
    dp->dp_matrix.score = NULL;

    gmatrix_step_destroy(dp->dp_matrix.step);
    dp->dp_matrix.step = NULL;

    free(dp->dp_matrix.state_col);

    free(dp);
}

int column(struct dp_matrix const* dp_matrix, struct step const* step)
{
    return dp_matrix->state_col[step->state->idx] + step->seq_len -
           imm_state_min_seq(step->state->state);
}

double get_score(struct dp_matrix const* dp_matrix, int row, struct step const* step)
{
    if (row < 0)
        return step->state->start_lprob;
    return matrix_get(dp_matrix->score, row, column(dp_matrix, step));
}

void set_score(struct dp_matrix const* dp_matrix, int row, struct step const* step,
               double score)
{
    matrix_set(dp_matrix->score, row, column(dp_matrix, step), score);
}

double best_trans_score(const struct dp* dp, struct state_info const* dst_state, int row,
                        struct step* prev_step)
{
    double score = LOG0;
    prev_step->state = NULL;
    prev_step->seq_len = -1;

    if (row == 0)
        score = dst_state->start_lprob;

    struct state_info const* prev = dp->states;
    for (int i = 0; i < dp->nstates; ++i, ++prev) {
        if (row - min_seq(prev) < 0)
            continue;

        double trans = matrix_get(dp->trans, i, dst_state->idx);

        for (int len = min_seq(prev); len <= MIN(max_seq(prev), row); ++len) {
            struct step tmp_step = {prev, len};
            int         prev_row = row - len;
            double      v = get_score(&dp->dp_matrix, prev_row, &tmp_step) + trans;
            if (v > score) {
                score = v;
                prev_step->state = prev;
                prev_step->seq_len = len;
            }
        }
    }
    return score;
}

double final_score(struct dp const* dp, struct step* end_step)
{
    double                   score = LOG0;
    struct state_info const* e = dp->end_state;
    end_step->state = dp->end_state;

    for (int len = MIN(imm_state_max_seq(e->state), dp->seq_len);
         imm_state_min_seq(e->state) <= len; --len) {

        struct step step = {e, len};
        double      s = get_score(&dp->dp_matrix, dp->seq_len - len, &step);
        if (s > score) {
            score = s;
            end_step->seq_len = len;
        }
    }
    return score;
}

int viterbi_path(struct dp* dp, struct imm_path* path, struct step const* end_step)
{
    return 0;
}

struct matrix* create_trans(const struct mm_state* const* mm_states, int nstates,
                            const struct state_idx* state_idx)
{
    struct matrix* trans = matrix_create(nstates, nstates);
    matrix_set_all(trans, LOG0);

    for (int i = 0; i < nstates; ++i) {

        int src = state_idx_find(state_idx, mm_state_get_state(mm_states[i]));
        const struct mm_trans* t = NULL;
        for (t = mm_state_get_trans_c(mm_states[i]); t; t = mm_trans_next_c(t)) {

            int dst = state_idx_find(state_idx, mm_trans_get_state(t));
            matrix_set(trans, src, dst, mm_trans_get_lprob(t));
        }
    }
    return trans;
}
