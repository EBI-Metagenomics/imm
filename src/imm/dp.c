#include "src/imm/dp.h"
#include "imm.h"
#include "src/imm/hide.h"
#include "src/imm/matrix.h"
#include "src/imm/min.h"
#include "src/imm/state_idx.h"
#include "src/imm/tbl_state.h"
#include "src/imm/tbl_trans.h"
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
    int state_id;
};

HIDE double get_score(const struct dp *dp, int row, int state_idx, int seq_len);
HIDE void set_score(const struct dp *dp, int row, int state_idx, int seq_len, double score);
HIDE double best_trans_score(const struct dp *dp, int row, int dst_state_idx);
HIDE struct matrix *create_trans(const struct tbl_state *tbl_states,
                                 const struct state_idx *state_idx);

struct dp *dp_create(const struct tbl_state *tbl_states, const char *seq, int seq_len)
{
    struct dp *dp = malloc(sizeof(struct dp));

    dp->nstates = tbl_state_nitems(tbl_states);
    dp->states = malloc(sizeof(struct state_info) * ((size_t)dp->nstates));

    struct state_idx *state_idx = NULL;
    state_idx_create(&state_idx);

    dp->seq = seq;
    dp->seq_len = seq_len;

    int next_col = 0;

    const struct tbl_state *tbl_state = tbl_states;
    for (int i = 0; i < dp->nstates; ++i) {
        dp->states[i].state = tbl_state_get_state(tbl_state);
        dp->states[i].start_lprob = tbl_state_get_start_lprob(tbl_state);
        dp->states[i].min_seq = imm_state_min_seq(dp->states[i].state);
        dp->states[i].max_seq = imm_state_max_seq(dp->states[i].state);

        dp->states[i].col = next_col;
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;

        dp->states[i].state_id = tbl_state_get_id(tbl_state);

        state_idx_add(&state_idx, dp->states[i].state_id, i);
        tbl_state = tbl_state_next_c(tbl_state);
    }

    dp->trans = create_trans(tbl_states, state_idx);
    state_idx_destroy(&state_idx);

    dp->score = matrix_create(seq_len, next_col);
    matrix_set_all(dp->score, -INFINITY);

    return dp;
}

double dp_viterbi(struct dp *dp)
{
    for (int r = 0; r < dp->seq_len; ++r) {
        const char *seq = dp->seq + r;
        int seq_len = dp->seq_len - r;

        for (int i = 0; i < dp->nstates; ++i) {
            const struct state_info *cur = dp->states + i;

            for (int len = cur->min_seq; len <= MIN(cur->max_seq, seq_len); ++len) {
                double emiss = imm_state_lprob(cur->state, seq, len);

                double score = best_trans_score(dp, r, i);
                set_score(dp, r, i, len, score + emiss);
            }
        }
    }

    matrix_print(dp->score);
    return 0.0;
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

void set_score(const struct dp *dp, int row, int state_idx, int seq_len, double score)
{
    int c = dp->states[state_idx].col + seq_len - dp->states[state_idx].min_seq;
    matrix_set(dp->score, row, c, score);
}

double get_score(const struct dp *dp, int row, int state_idx, int seq_len)
{
    if (row < 0)
        return dp->states[state_idx].start_lprob;
    int c = dp->states[state_idx].col + seq_len - dp->states[state_idx].min_seq;
    return matrix_get(dp->score, row, c);
}

double best_trans_score(const struct dp *dp, int row, int dst_state_idx)
{
    double score = -INFINITY;
    int seq_len = dp->seq_len - row;
    for (int i = 0; i < dp->nstates; ++i) {
        const struct state_info *state = dp->states + i;
        double trans = matrix_get(dp->trans, i, dst_state_idx);

        for (int len = state->min_seq; len <= MIN(state->max_seq, seq_len); ++len) {
            double v = get_score(dp, row - len, i, len) + trans;
            score = MAX(v, score);
        }
    }
    return score;
}

struct matrix *create_trans(const struct tbl_state *tbl_states,
                            const struct state_idx *state_idx)
{
    int nstates = tbl_state_nitems(tbl_states);
    struct matrix *trans = matrix_create(nstates, nstates);
    matrix_set_all(trans, -INFINITY);

    for (const struct tbl_state *s = tbl_states; s; s = tbl_state_next_c(s)) {

        int src = state_idx_find(state_idx, tbl_state_get_id(s));
        const struct tbl_trans *t = NULL;
        for (t = tbl_state_get_trans_c(s); t; t = tbl_trans_next_c(t)) {

            int dst = state_idx_find(state_idx, tbl_trans_get_id(t));
            matrix_set(trans, src, dst, tbl_trans_get_lprob(t));
        }
    }
    return trans;
}
