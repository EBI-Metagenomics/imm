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
    struct state_idx *state_idx;

    const char *seq;
    int seq_len;

    struct matrix *trans;
    struct matrix *matrix;
};

double get_cost(const struct matrix *matrix, int r, int c);

struct dp *dp_create(const struct tbl_state *tbl_states, const char *seq,
                     size_t seq_len)
{
    if (seq_len > INT_MAX) {
        error("`seq_len` is greater than `INT_MAX`");
        return NULL;
    }

    struct dp *dp = malloc(sizeof(struct dp));
    size_t nstates = (size_t)tbl_state_nitems(tbl_states);
    dp->state_col = malloc(sizeof(int) * nstates);
    dp->states = malloc(sizeof(struct imm_state *) * nstates);
    state_idx_create(&dp->state_idx);

    int next_state_col = 0;

    const struct tbl_state *tbl_state = tbl_states;
    for (size_t i = 0; i < nstates; ++i) {
        dp->state_col[i] = next_state_col;
        dp->states[i] = tbl_state_get_state(tbl_state);

        size_t max_seq = state_max_seq(dp->states[i]);
        size_t min_seq = state_min_seq(dp->states[i]);
        next_state_col += max_seq - min_seq + 1;

        state_idx_add(&dp->state_idx, tbl_state_get_id(tbl_state), (int)i);
        tbl_state = tbl_state_next_c(tbl_state);
    }

    dp->seq = seq;
    dp->seq_len = (int)seq_len;
    dp->matrix = matrix_create(seq_len, (size_t)next_state_col);

    dp->trans = matrix_create(nstates, nstates);
    matrix_set_all(dp->trans, -INFINITY);
    tbl_state = tbl_states;
    for (size_t i = 0; i < nstates; ++i) {
        int src = state_idx_find(dp->state_idx, tbl_state_get_id(tbl_state));

        for (const struct tbl_trans *tbl_trans = tbl_state_get_trans_c(tbl_state);
             tbl_trans; tbl_trans = tbl_trans_next_c(tbl_trans)) {
            int dst = state_idx_find(dp->state_idx, tbl_trans_get_id(tbl_trans));
            double trans_lprob = tbl_trans_get_lprob(tbl_trans);
            matrix_set(dp->trans, (size_t)src, (size_t)dst, trans_lprob);
        }

        tbl_state = tbl_state_next_c(tbl_state);
    }

    return dp;
}

double dp_viterbi(struct dp *dp)
{
    /* for (size_t row = 0; row < dp->seq_len; ++row) { */

    /*     size_t state_idx = 0; */
    /*     for (const struct tbl_state *tbl_state = dp->tbl_states; tbl_state; */
    /*          tbl_state = tbl_state_next_c(tbl_state)) { */

    /*         size_t col = dp->state_col[state_idx]; */
    /*         const struct imm_state *state = tbl_state_get_state(tbl_state); */
    /*         size_t max_len = MIN(state_max_seq(state), (size_t)(dp->seq_len - row));
     */

    /*         for (size_t len = state_min_seq(state); len <= max_len; ++len) { */

    /*             /1* double get_cost(const struct matrix *matrix, int r, int c) *1/ */
    /*             double cost = imm_state_emiss_lprob(state, dp->seq + row, len); */
    /*             matrix_set(dp->matrix, row, col, cost); */
    /*             ++col; */
    /*         } */

    /*         ++state_idx; */
    /*     } */
    /* } */

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
    return matrix_get(matrix, (size_t)r, (size_t)c);
}
