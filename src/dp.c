#include "dp.h"
#include "limits.h"
#include "macro.h"
#include "matrix.h"
#include "report.h"
#include "state/state.h"
#include "state_idx.h"
#include "tbl_state.h"
#include "tbl_trans.h"
#include "uthash.h"
#include <math.h>

struct dp
{
    int *state_col;
    const struct imm_state **states;
    int nstates;
    struct state_idx *state_idx;

    const char *seq;
    int seq_len;

    struct matrix *trans;
    struct matrix *matrix;
};

double get_cost(const struct matrix *matrix, int r, int c);
int set_states(struct dp *dp, const struct tbl_state *tbl_states);
void set_trans(struct dp *dp, const struct tbl_state *tbl_states);

struct dp *dp_create(const struct tbl_state *tbl_states, const char *seq,
                     int seq_len)
{
    if (seq_len > INT_MAX) {
        error("`seq_len` is greater than `INT_MAX`");
        return NULL;
    }

    struct dp *dp = malloc(sizeof(struct dp));
    dp->seq = seq;
    dp->seq_len = (int)seq_len;

    int ncols = set_states(dp, tbl_states);
    if (ncols < 0) {
        free(dp);
        return NULL;
    }
    dp->matrix = matrix_create(seq_len, (int)ncols);
    set_trans(dp, tbl_states);

    return dp;
}

double dp_viterbi(struct dp *dp)
{
    int seq_len = dp->seq_len;
    const char *seq = dp->seq;
    for (int r = 0; r < dp->seq_len; ++r) {

        for (int state_idx = 0; state_idx < dp->nstates; ++state_idx) {
            const struct imm_state *state = dp->states[state_idx];

            int max_len = MIN(state_max_seq(state), (int)seq_len);
            int min_len = MIN(state_min_seq(state), (int)seq_len);

            int c = dp->state_col[state_idx];
            for (int len = min_len; len <= max_len; ++len) {
                double emiss_cost = imm_state_emiss_lprob(state, dp->seq + r, len);

                for (int pstate_idx = 0; pstate_idx < dp->nstates; ++pstate_idx) {
                    double trans_cost = matrix_get(dp->trans, pstate_idx, state_idx);
                    const struct imm_state *pstate = dp->states[pstate_idx];
                    int pmax_len = state_max_seq(pstate);
                    pmax_len = MIN(pmax_len, (int)(dp->seq_len - r));
                    int pmin_len = state_min_seq(pstate);
                    pmin_len = MIN(pmin_len, (int)(dp->seq_len - r));
                    int pc = dp->state_col[pstate_idx];
                    for (int plen = pmin_len; plen <= pmax_len; ++plen) {

                        double pcost =
                            get_cost(dp->matrix, r - plen, pc + (plen - pmin_len));
                        matrix_set(dp->matrix, r, c,
                                   MIN(matrix_get(dp->matrix, r, c),
                                       pcost + trans_cost + emiss_cost));
                    }
                }
            }
        }
        ++seq;
        --seq_len;
    }

    return 0.0;
}

void dp_destroy(struct dp *dp)
{
    if (!dp)
        return;

    free(dp->state_col);
    dp->state_col = NULL;

    matrix_destroy(dp->matrix);
    dp->matrix = NULL;

    free(dp);
}

double get_cost(const struct matrix *matrix, int r, int c)
{
    if (r < 0 || c < 0)
        return -INFINITY;
    return matrix_get(matrix, (int)r, (int)c);
}

int set_states(struct dp *dp, const struct tbl_state *tbl_states)
{
    int nstates = (int)tbl_state_nitems(tbl_states);
    dp->state_col = malloc(sizeof(int) * ((size_t) nstates));
    dp->states = malloc(sizeof(struct imm_state *) * ((size_t) nstates));
    state_idx_create(&dp->state_idx);

    int next_state_col = 0;

    const struct tbl_state *tbl_state = tbl_states;
    for (int i = 0; i < nstates; ++i) {
        dp->state_col[i] = next_state_col;
        dp->states[i] = tbl_state_get_state(tbl_state);

        int max_seq = state_max_seq(dp->states[i]);
        int min_seq = state_min_seq(dp->states[i]);
        next_state_col += max_seq - min_seq + 1;

        state_idx_add(&dp->state_idx, tbl_state_get_id(tbl_state), (int)i);
        tbl_state = tbl_state_next_c(tbl_state);
    }

    if (nstates > INT_MAX) {
        error("`nstates` is greater than `INT_MAX`");
        free(dp->state_col);
        free(dp->states);
        state_idx_destroy(&dp->state_idx);
        return -1;
    }
    dp->nstates = (int)nstates;
    return next_state_col;
}

void set_trans(struct dp *dp, const struct tbl_state *tbl_states)
{
    int nstates = (int)tbl_state_nitems(tbl_states);
    dp->trans = matrix_create(nstates, nstates);
    matrix_set_all(dp->trans, -INFINITY);

    for (const struct tbl_state *s = tbl_states; s; s = tbl_state_next_c(s)) {

        int src = state_idx_find(dp->state_idx, tbl_state_get_id(s));
        const struct tbl_trans *t = NULL;
        for (t = tbl_state_get_trans_c(s); t; t = tbl_trans_next_c(t)) {

            int dst = state_idx_find(dp->state_idx, tbl_trans_get_id(t));
            double trans_lprob = tbl_trans_get_lprob(t);
            matrix_set(dp->trans, (int)src, (int)dst, trans_lprob);
        }
    }
}
