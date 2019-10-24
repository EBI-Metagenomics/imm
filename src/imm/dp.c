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
    const struct imm_state *state;
    double start_lprob;
    int idx;
};

struct step
{
    struct state_info const *state;
    int seq_len;
};
MAKE_GMATRIX_STRUCT(step, struct step)
MAKE_GMATRIX_CREATE(step, struct step)
MAKE_GMATRIX_GET(step, struct step)
MAKE_GMATRIX_DESTROY(step, struct step)

struct dp_matrix
{
    struct matrix *score;
    struct gmatrix_step *step;
    int *state_col;
};

struct dp
{
    int nstates;
    struct state_info *states;
    struct state_info const *end_state;

    const char *seq;
    int seq_len;

    struct matrix *trans;

    struct dp_matrix dp_matrix;
};

static inline int column(struct dp_matrix const *dp_matrix, struct state_info const *state,
                         int seq_len)
{
    return dp_matrix->state_col[state->idx] + seq_len - imm_state_min_seq(state->state);
}

HIDE double get_score(struct dp_matrix const *dp_matrix, int row,
                      struct state_info const *state, int seq_len);
HIDE void set_score(struct dp_matrix const *dp_matrix, int row,
                    struct state_info const *state, int seq_len, double score);
HIDE double best_trans_score(const struct dp *dp, struct state_info const *state, int row,
                             struct step *step);
HIDE struct matrix *create_trans(const struct mm_state *const *mm_states, int nstates,
                                 const struct state_idx *state_idx);
HIDE double final_score(struct dp const *dp, int *seq_len);
HIDE int viterbi_path(struct dp *dp, struct imm_path *path, int end_seq_len);

struct dp *dp_create(const struct mm_state *const *mm_states, int nstates, const char *seq,
                     struct imm_state const *end_state)
{
    struct dp *dp = malloc(sizeof(struct dp));

    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)nstates));

    struct state_idx *state_idx = NULL;
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

double dp_viterbi(struct dp *dp, struct imm_path *path)
{
    for (int r = 0; r <= dp->seq_len; ++r) {
        char const *seq = dp->seq + r;
        int seq_len = dp->seq_len - r;

        struct state_info const *cur = dp->states;
        for (int i = 0; i < dp->nstates; ++i, ++cur) {

            for (int len = imm_state_min_seq(cur->state);
                 len <= MIN(imm_state_max_seq(cur->state), seq_len); ++len) {

                int col = column(&dp->dp_matrix, cur, len);
                struct step *step = gmatrix_step_get(dp->dp_matrix.step, r, col);
                double score = best_trans_score(dp, cur, r, step);
                double emiss = imm_state_lprob(cur->state, seq, len);
                set_score(&dp->dp_matrix, r, cur, len, score + emiss);
            }
        }
    }

    int end_seq_len = -1;
    double score = final_score(dp, &end_seq_len);

    if (path) {
        if (viterbi_path(dp, path, end_seq_len))
            return NAN;
    }

    return score;
}

void dp_destroy(struct dp *dp)
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

void set_score(struct dp_matrix const *dp_matrix, int row, struct state_info const *state,
               int seq_len, double score)
{
    matrix_set(dp_matrix->score, row, column(dp_matrix, state, seq_len), score);
}

double get_score(struct dp_matrix const *dp_matrix, int row, struct state_info const *state,
                 int seq_len)
{
    if (row < 0)
        return state->start_lprob;
    return matrix_get(dp_matrix->score, row, column(dp_matrix, state, seq_len));
}

double best_trans_score(const struct dp *dp, struct state_info const *state, int row,
                        struct step *step)
{
    double score = LOG0;
    step->state = NULL;
    step->seq_len = -1;

    if (row == 0)
        score = state->start_lprob;

    for (int i = 0; i < dp->nstates; ++i) {
        struct state_info const *prev = dp->states + i;
        if (row - imm_state_min_seq(prev->state) < 0)
            continue;

        double trans = matrix_get(dp->trans, i, state->idx);

        for (int len = imm_state_min_seq(prev->state);
             len <= MIN(imm_state_max_seq(prev->state), row); ++len) {
            double v = get_score(&dp->dp_matrix, row - len, prev, len) + trans;
            if (v > score) {
                score = v;
                step->state = prev;
                step->seq_len = len;
            }
        }
    }
    return score;
}

struct matrix *create_trans(const struct mm_state *const *mm_states, int nstates,
                            const struct state_idx *state_idx)
{
    struct matrix *trans = matrix_create(nstates, nstates);
    matrix_set_all(trans, LOG0);

    for (int i = 0; i < nstates; ++i) {

        int src = state_idx_find(state_idx, mm_state_get_state(mm_states[i]));
        const struct mm_trans *t = NULL;
        for (t = mm_state_get_trans_c(mm_states[i]); t; t = mm_trans_next_c(t)) {

            int dst = state_idx_find(state_idx, mm_trans_get_state(t));
            matrix_set(trans, src, dst, mm_trans_get_lprob(t));
        }
    }
    return trans;
}

double final_score(struct dp const *dp, int *seq_len)
{
    double score = LOG0;
    struct state_info const *e = dp->end_state;

    for (int len = MIN(imm_state_max_seq(e->state), dp->seq_len);
         imm_state_min_seq(e->state) <= len; --len) {
        double s = get_score(&dp->dp_matrix, dp->seq_len - len, e, len);
        if (s > score) {
            score = s;
            *seq_len = len;
        }
    }
    return score;
}

int viterbi_path(struct dp *dp, struct imm_path *path, int end_seq_len) { return 0; }
