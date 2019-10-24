#include "src/imm/dp.h"
#include "imm.h"
#include "src/imm/hide.h"
#include "src/imm/matrix.h"
#include "src/imm/min.h"
#include "src/imm/mm_state.h"
#include "src/imm/mm_trans.h"
#include "src/imm/state_idx.h"
#include "src/uthash/uthash.h"
#include <limits.h>
#include <math.h>

struct dp
{
    int nstates;
    struct state_info *states;

    const char *seq;
    int seq_len;

    struct matrix *trans;
    struct matrix *score;
};

struct state_info
{
    const struct imm_state *state;
    double start_lprob;
    int min_seq;
    int max_seq;
    int col;
};

static inline int column(struct state_info const *state, int seq_len)
{
    return state->col + seq_len - state->min_seq;
}

HIDE double get_score(const struct dp *dp, int row, struct state_info const *state,
                      int seq_len);
HIDE void set_score(const struct dp *dp, int row, struct state_info const *state, int seq_len,
                    double score);
HIDE double best_trans_score(const struct dp *dp, double start_lprob, int row,
                             int dst_state_idx);
HIDE struct matrix *create_trans(const struct mm_state *const *mm_states, int nstates,
                                 const struct state_idx *state_idx);
HIDE int viterbi_path(struct dp *dp, struct imm_state const *end_state,
                      struct imm_path *path);

struct dp *dp_create(const struct mm_state *const *mm_states, int nstates, const char *seq)
{
    struct dp *dp = malloc(sizeof(struct dp));

    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)dp->nstates));

    struct state_idx *state_idx = NULL;
    state_idx_create(&state_idx);

    dp->seq = seq;
    dp->seq_len = (int)strlen(seq);

    int next_col = 0;

    for (int i = 0; i < dp->nstates; ++i) {
        dp->states[i].state = mm_state_get_state(mm_states[i]);
        dp->states[i].start_lprob = mm_state_get_start_lprob(mm_states[i]);
        dp->states[i].min_seq = imm_state_min_seq(dp->states[i].state);
        dp->states[i].max_seq = imm_state_max_seq(dp->states[i].state);

        dp->states[i].col = next_col;
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;

        dp->states[i].state = mm_state_get_state(mm_states[i]);

        state_idx_add(&state_idx, dp->states[i].state, i);
    }

    dp->trans = create_trans(mm_states, nstates, state_idx);
    state_idx_destroy(&state_idx);

    dp->score = matrix_create(dp->seq_len + 1, next_col);
    matrix_set_all(dp->score, LOG0);

    return dp;
}

double dp_viterbi(struct dp *dp, const struct imm_state *end_state, struct imm_path *path)
{
    for (int r = 0; r <= dp->seq_len; ++r) {
        char const *seq = dp->seq + r;
        int seq_len = dp->seq_len - r;

        struct state_info const *cur = dp->states;
        for (int i = 0; i < dp->nstates; ++i, ++cur) {

            for (int len = cur->min_seq; len <= MIN(cur->max_seq, seq_len); ++len) {

                double score = best_trans_score(dp, cur->start_lprob, r, i);
                double emiss = imm_state_lprob(cur->state, seq, len);
                set_score(dp, r, cur, len, score + emiss);
            }
        }
    }

    double score = LOG0;
    const struct state_info *cur = dp->states;
    for (int i = 0; i < dp->nstates; ++i, ++cur) {
        if (cur->state == end_state) {

            for (int len = MIN(cur->max_seq, dp->seq_len); cur->min_seq <= len; --len) {
                score = MAX(score, get_score(dp, dp->seq_len - len, cur, len));
            }
            break;
        }
    }

    if (path) {
        if (viterbi_path(dp, end_state, path))
            return NAN;
    }

    return score;
}

void dp_destroy(struct dp *dp)
{
    if (!dp)
        return;

    free(dp->states);
    dp->states = NULL;

    dp->seq = NULL;
    dp->seq_len = -1;

    matrix_destroy(dp->trans);
    dp->trans = NULL;

    matrix_destroy(dp->score);
    dp->score = NULL;

    free(dp);
}

void set_score(const struct dp *dp, int row, struct state_info const *state, int seq_len,
               double score)
{
    matrix_set(dp->score, row, column(state, seq_len), score);
}

double get_score(const struct dp *dp, int row, struct state_info const *state, int seq_len)
{
    if (row < 0)
        return state->start_lprob;
    return matrix_get(dp->score, row, column(state, seq_len));
}

double best_trans_score(const struct dp *dp, double start_lprob, int row, int dst_state_idx)
{
    double score = LOG0;
    if (row == 0)
        score = start_lprob;
    for (int i = 0; i < dp->nstates; ++i) {
        struct state_info const *state = dp->states + i;
        if (row - state->min_seq < 0)
            continue;

        double trans = matrix_get(dp->trans, i, dst_state_idx);

        for (int len = state->min_seq; len <= MIN(state->max_seq, row); ++len) {
            double v = get_score(dp, row - len, state, len) + trans;
            score = MAX(v, score);
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

int viterbi_path(struct dp *dp, struct imm_state const *end_state, struct imm_path *path)
{

    return 0;
}
