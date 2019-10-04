#include "state/state.h"
#include "tbl_state.h"
#include "matrix.h"
#include <stddef.h>

struct dp
{
    size_t *state_col;
    const struct tbl_state *tb_states;
    const char *seq;
    size_t seq_len;
    struct matrix *matrix;
};

struct dp* dp_create(const struct tbl_state *tbl_states, const char *seq, size_t seq_len)
{
    struct dp* dp = malloc(sizeof(struct dp));
    dp->tbl_states = tbl_states;

    const struct tbl_state *tbl_state = tbl_states;
    size_t state_col = 0;
    while (tbl_state) {
        state_space += state_max_seq(tbl_state_get_state(tbl_state));
        tbl_state = tbl_state_next_c(tbl_state);
    }
    return dp;
}

void dp_viterbi(void)
{

    struct matrix *matrix = matrix_create(seq_len, state_space);

    for (size_t r = 0; r < seq_len; ++r) {

        tbl_state = hmm->tbl_states;
        size_t state_idx = 0;
        size_t state_offset = 0;
        while (tbl_state) {

            const struct imm_state *state = tbl_state_get_state(tbl_state);
            size_t max_len = MIN(state_max_seq(state), seq_len - r);
            for (size_t len = state_min_seq(state); len <= max_len; ++len) {

                matrix_set(matrix, r, c);
                imm_state_emiss_lprob(state, seq + r, len);
            }

            tbl_state = tbl_state_next_c(tbl_state);
            ++state_idx;
        }
    }

    matrix_destroy(matrix);

    return 0.0;

}

void dp_destroy(struct dp* dp)
{
    if (!dp)
        return;

    free(dp);
}
