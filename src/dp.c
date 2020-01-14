#include "dp.h"
#include "elapsed.h"
#include "gmatrix.h"
#include "hide.h"
#include "imm/imm.h"
#include "list.h"
#include "matrix.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "state_idx.h"
#include "uthash.h"
#include <limits.h>
#include <math.h>
#include <stdio.h>

struct trans;

struct state_info
{
    struct imm_state const* state;
    int                     min_seq;
    int                     max_seq;
    double                  start_lprob;
    int                     idx;
    struct list_head        incoming_transitions;
};

struct trans
{
    struct state_info const* src_state;
    double                   lprob;
    struct list_head         list_entry;
};

/* static inline int min_seq(struct state_info const* state_info) */
/* { */
/*     return imm_state_min_seq(state_info->state); */
/* } */
/* static inline int max_seq(struct state_info const* state_info) */
/* { */
/*     return imm_state_max_seq(state_info->state); */
/* } */

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
MAKE_GMATRIX_DESTROY(cell, struct cell)

struct dp_matrix
{
    struct gmatrix_cell* cell;
    int*                 state_col;
};

struct dp
{
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
static double get_score(struct dp_matrix const* dp_matrix, int row, struct step const* step);
static void   set_score(struct dp_matrix const* dp_matrix, int row, struct step const* step,
                        double score);

static double best_trans_score(struct dp const* dp, struct state_info const* dst_state,
                               int row, struct step* prev_step);
static double final_score(struct dp const* dp, struct step* end_step);
static void   viterbi_path(struct dp* dp, struct imm_path* path, struct step const* end_step);

static void create_trans(struct state_info* states, struct mstate const* const* mm_states,
                         int nstates, struct state_idx const* state_idx);

struct dp* imm_dp_create(struct mstate const* const* mm_states, int nstates, char const* seq,
                         struct imm_state const* end_state)
{
    struct elapsed* elapsed = elapsed_create();

    elapsed_start(elapsed);
    struct dp* dp = malloc(sizeof(struct dp));

    dp->nstates = nstates;
    dp->states = malloc(sizeof(struct state_info) * ((size_t)nstates));

    struct state_idx* state_idx = NULL;
    imm_state_idx_create(&state_idx);

    dp->seq = seq;
    dp->seq_len = (int)strlen(seq);

    dp->dp_matrix.state_col = malloc(sizeof(int) * ((size_t)nstates));
    dp->dp_matrix.cell = NULL;
    int next_col = 0;

    printf("imm_dp_create mallocs: %.10f seconds\n", elapsed_end(elapsed));
    elapsed_start(elapsed);
    for (int i = 0; i < nstates; ++i) {
        dp->states[i].state = imm_mstate_get_state(mm_states[i]);
        dp->states[i].min_seq = imm_state_min_seq(dp->states[i].state);
        dp->states[i].max_seq = imm_state_max_seq(dp->states[i].state);
        dp->states[i].start_lprob = imm_mstate_get_start(mm_states[i]);
        dp->states[i].idx = i;
        INIT_LIST_HEAD(&dp->states[i].incoming_transitions);

        dp->dp_matrix.state_col[i] = next_col;

        imm_state_idx_add(&state_idx, dp->states[i].state, i);
        next_col += dp->states[i].max_seq - dp->states[i].min_seq + 1;
    }
    dp->end_state = dp->states + imm_state_idx_find(state_idx, end_state);
    printf("imm_dp_create loop: %.10f seconds\n", elapsed_end(elapsed));

    elapsed_start(elapsed);
    create_trans(dp->states, mm_states, nstates, state_idx);
    printf("imm_dp_create create_trans: %.10f seconds\n", elapsed_end(elapsed));
    imm_state_idx_destroy(&state_idx);

    /* elapsed_start(elapsed); */
    /* dp->dp_matrix.score = imm_matrix_create(dp->seq_len + 1, next_col); */
    /* printf("imm_dp_create imm_matrix_create: %.10f seconds\n", elapsed_end(elapsed)); */
    elapsed_start(elapsed);
    dp->dp_matrix.cell = gmatrix_cell_create(dp->seq_len + 1, next_col);
    printf("imm_dp_create gmatrix_step_create: %.10f seconds\n", elapsed_end(elapsed));

    elapsed_destroy(elapsed);
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
                int          col = column(&dp->dp_matrix, &step);
                struct step* pstep = &gmatrix_cell_get(dp->dp_matrix.cell, r, col)->prev_step;
                double       score = best_trans_score(dp, cur, r, pstep);
                double       emiss = imm_state_lprob(cur->state, seq, len);
                set_score(&dp->dp_matrix, r, &step, score + emiss);
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
    for (int i = 0; i < dp->nstates; ++i) {
        struct trans* trans = NULL;
        list_for_each_entry(trans, &dp->states[i].incoming_transitions, list_entry)
        {
            free(trans);
        }
    }

    dp->nstates = -1;

    free(dp->states);
    dp->states = NULL;

    dp->end_state = NULL;

    dp->seq = NULL;
    dp->seq_len = -1;

    /* imm_matrix_destroy(dp->trans); */
    /* dp->trans = NULL; */

    /* imm_matrix_destroy(dp->dp_matrix.score); */
    /* dp->dp_matrix.score = NULL; */

    gmatrix_cell_destroy(dp->dp_matrix.cell);
    dp->dp_matrix.cell = NULL;

    free(dp->dp_matrix.state_col);

    free(dp);
}

static double get_score(struct dp_matrix const* dp_matrix, int row, struct step const* step)
{
    if (row < 0)
        return step->state->start_lprob;
    return gmatrix_cell_get(dp_matrix->cell, row, column(dp_matrix, step))->score;
    /* return imm_matrix_get(dp_matrix->score, row, column(dp_matrix, step)); */
}

static void set_score(struct dp_matrix const* dp_matrix, int row, struct step const* step,
                      double score)
{
    gmatrix_cell_get(dp_matrix->cell, row, column(dp_matrix, step))->score = score;
    /* imm_matrix_set(dp_matrix->score, row, column(dp_matrix, step), score); */
}

static double best_trans_score(struct dp const* dp, struct state_info const* dst_state,
                               int row, struct step* prev_step)
{
    double score = imm_lprob_zero();
    prev_step->state = NULL;
    prev_step->seq_len = -1;

    if (row == 0)
        score = dst_state->start_lprob;

    struct trans* trans = NULL;
    list_for_each_entry(trans, &dst_state->incoming_transitions, list_entry)
    {
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
    double                   score = imm_lprob_zero();
    struct state_info const* end_state = dp->end_state;

    end_step->state = NULL;
    end_step->seq_len = -1;

    for (int len = MIN(end_state->max_seq, dp->seq_len); end_state->min_seq <= len; --len) {

        struct step step = {.state = end_state, .seq_len = len};
        double      s = get_score(&dp->dp_matrix, dp->seq_len - len, &step);
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
        step = &gmatrix_cell_get(dp->dp_matrix.cell, row, column(&dp->dp_matrix, step))
                    ->prev_step;
    }
}

static void create_trans(struct state_info* states, struct mstate const* const* mm_states,
                         int nstates, struct state_idx const* state_idx)
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

            struct trans* trans = malloc(sizeof(struct trans));
            trans->lprob = lprob;
            trans->src_state = states + src;
            list_add(&trans->list_entry, &states[dst].incoming_transitions);
        }
    }
}
