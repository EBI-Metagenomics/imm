#include "dp.h"
#include "compiler.h"
#include "dp_emission.h"
#include "dp_matrix.h"
#include "dp_state_table.h"
#include "dp_step.h"
#include "dp_trans_table.h"
/* #include "elapsed.h" */
#include "free.h"
#include "imm/dp.h"
#include "imm/lprob.h"
#include "imm/path.h"
#include "imm/report.h"
#include "imm/results.h"
#include "imm/state.h"
#include "imm/step.h"
#include "imm/subseq.h"
#include "imm/window.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "seq_code.h"
#include "state_idx.h"
#include "thread.h"
#include <limits.h>
#include <stdio.h>

struct imm_dp
{
    struct mstate const* const*  mstates;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table const* trans_table;
    struct dp_state_table const* state_table;
    struct dp_matrix**           matrices;
    struct eseq**                eseqs;
    unsigned                     max_nthreads;
};

static double   viterbi(struct imm_dp const* dp, struct dp_matrix* matrix,
                        struct eseq const* eseq, struct imm_path* path);
static double   best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                 unsigned target_state, unsigned row, struct dp_step* prev_step);
static double   final_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                            struct dp_step* end_step, struct eseq const* eseq);
static void     viterbi_path(struct imm_dp const* dp, struct dp_matrix const* matrix,
                             struct imm_path* path, struct dp_step const* end_step,
                             struct eseq const* eseq);
static unsigned min_seq(struct mstate const* const* mstates, uint32_t nstates);
static unsigned max_seq(struct mstate const* const* mstates, uint32_t nstates);

static inline void set_score(struct imm_dp const* dp, struct dp_matrix* matrix,
                             struct eseq const* eseq, double trans_score, unsigned min_len,
                             unsigned max_len, unsigned row, unsigned state)
{
    for (unsigned len = min_len; len <= max_len; ++len) {
        struct dp_step step = {.state = state, .seq_len = len};

        unsigned seq_code = eseq_get(eseq, row, len);
        seq_code -= seq_code_offset(dp->seq_code, min_len);

        double score = trans_score + dp_emission_score(dp->emission, state, seq_code);
        dp_matrix_set_score(matrix, row, step, score);
    }
}

struct imm_results const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_seq const* seq,
                                         unsigned window_length)
{
    if (seq_code_abc(dp->seq_code) != imm_seq_get_abc(seq)) {
        imm_error("dp and seq must have the same alphabet");
        return NULL;
    }

    struct imm_state const* end_state =
        mstate_get_state(dp->mstates[dp->state_table->end_state]);
    if (imm_seq_length(seq) < imm_state_min_seq(end_state)) {
        imm_error("sequence is shorter than end_state's lower bound");
        return NULL;
    }

    /* struct elapsed* elapsed = elapsed_create(); */

    /* elapsed_start(elapsed); */
    /* fprintf(stderr, "hmm_create_dp: %f seconds\n", elapsed_end(elapsed)); */
    if (!dp) {
        /* elapsed_destroy(elapsed); */
        return NULL;
    }

    if (window_length == 0)
        window_length = imm_seq_length(seq);

    /* elapsed_start(elapsed); */
    struct imm_window*  window = imm_window_create(seq, window_length);
    unsigned const      nwindows = imm_window_size(window);
    struct imm_results* results = imm_results_create(seq, nwindows);
    /* fprintf(stderr, "window init  : %f seconds\n", elapsed_end(elapsed)); */

    /* struct elapsed* elapsed1 = elapsed_create(); */

    _Pragma("omp parallel if(nwindows > 1)")
    {
        /* elapsed_start(elapsed); */
        if (!dp->matrices[thread_id()])
            dp->matrices[thread_id()] = dp_matrix_create(dp->state_table);

        if (!dp->eseqs[thread_id()])
            dp->eseqs[thread_id()] = seq_code_create_eseq(dp->seq_code);

        _Pragma("omp single")
        {
            for (unsigned i = 0; i < nwindows; ++i) {
                struct imm_subseq subseq = imm_window_next(window);
                _Pragma("omp task firstprivate(subseq, i)")
                {
                    struct imm_path* path = imm_path_create();

                    struct dp_matrix*  matrix = dp->matrices[thread_id()];
                    struct eseq const* eseq = dp->eseqs[thread_id()];

                    eseq_setup(eseq, imm_subseq_cast(&subseq));

                    /* elapsed_start(elapsed1); */
                    /* fprintf(stderr, "create_eseq  : %f seconds\n", elapsed_end(elapsed1));
                     */

                    /* elapsed_start(elapsed1); */
                    dp_matrix_setup(matrix, eseq);
                    /* fprintf(stderr, "matrix_setup : %f seconds\n", elapsed_end(elapsed1));
                     */

                    /* elapsed_start(elapsed1); */
                    double score = viterbi(dp, matrix, eseq, path);
                    /* fprintf(stderr, "dp_viterbi   : %f seconds\n", elapsed_end(elapsed1));
                     */

                    imm_results_set(results, i, subseq, path, score);
                }
            }
            imm_window_destroy(window);
        }
        /* fprintf(stderr, "main loop    : %f seconds\n", elapsed_end(elapsed)); */
    }

    /* elapsed_destroy(elapsed); */
    /* elapsed_destroy(elapsed1); */
    return results;
}

void imm_dp_destroy(struct imm_dp const* dp)
{
    free_c(dp->mstates);
    seq_code_destroy(dp->seq_code);
    dp_emission_destroy(dp->emission);
    dp_trans_table_destroy(dp->trans_table);
    dp_state_table_destroy(dp->state_table);

    for (unsigned i = 0; i < dp->max_nthreads; ++i) {

        if (dp->matrices[i])
            dp_matrix_destroy(dp->matrices[i]);

        if (dp->eseqs[i])
            eseq_destroy(dp->eseqs[i]);
    }

    free_c(dp->matrices);
    free_c(dp->eseqs);
    free_c(dp);
}

struct imm_dp const* dp_create(struct imm_abc const* abc, struct mstate const* const* mstates,
                               uint32_t nstates, struct imm_state const* end_state)
{
    struct imm_dp* dp = malloc(sizeof(*dp));
    dp->mstates = mstates;
    dp->seq_code = seq_code_create(abc, min_seq(mstates, nstates), max_seq(mstates, nstates));

    struct state_idx* state_idx = state_idx_create();
    dp->state_table = dp_state_table_create(mstates, nstates, end_state, state_idx);
    dp->emission = dp_emission_create(dp->seq_code, mstates, nstates);
    dp->trans_table = dp_trans_table_create(mstates, nstates, state_idx);
    state_idx_destroy(state_idx);

    dp->max_nthreads = thread_max_size();
    dp->matrices = malloc(sizeof(struct dp_matrix*) * dp->max_nthreads);
    dp->eseqs = malloc(sizeof(struct eseq*) * dp->max_nthreads);

    for (unsigned i = 0; i < dp->max_nthreads; ++i) {
        dp->matrices[i] = NULL;
        dp->eseqs[i] = NULL;
    }

    return dp;
}

int dp_write(struct imm_dp const* dp, FILE* stream)
{
    if (seq_code_write(dp->seq_code, stream)) {
        imm_error("could not write seq_code");
        return 1;
    }

    if (dp_emission_write(dp->emission, dp_state_table_nstates(dp->state_table), stream)) {
        imm_error("could not write dp_emission");
        return 1;
    }

    if (dp_trans_table_write(dp->trans_table, dp_state_table_nstates(dp->state_table),
                             stream)) {
        imm_error("could not write dp_trans");
        return 1;
    }

    return 0;
}

struct mstate const* const* dp_get_mstates(struct imm_dp const* dp) { return dp->mstates; }

struct dp_state_table const* dp_get_dp_states(struct imm_dp const* dp)
{
    return dp->state_table;
}

struct dp_trans_table const* dp_get_dp_trans(struct imm_dp const* dp)
{
    return dp->trans_table;
}

static double viterbi(struct imm_dp const* dp, struct dp_matrix* matrix,
                      struct eseq const* eseq, struct imm_path* path)
{
    unsigned seq_len = eseq_length(eseq);

    for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

        unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
        unsigned max_len = MIN(dp_state_table_max_seq(dp->state_table, i), seq_len);

        struct dp_step* prev_step = dp_matrix_get_prev_step(matrix, 0, i);
        double          tscore = best_trans_score(dp, matrix, i, 0, prev_step);

        tscore = MAX(dp_state_table_start_lprob(dp->state_table, i), tscore);
        set_score(dp, matrix, eseq, tscore, min_len, max_len, 0, i);
    }

    for (unsigned r = 1; r <= eseq_length(eseq); ++r) {
        --seq_len;

        for (unsigned i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
            unsigned max_len = MIN(dp_state_table_max_seq(dp->state_table, i), seq_len);

            struct dp_step* prev_step = dp_matrix_get_prev_step(matrix, r, i);
            double          tscore = best_trans_score(dp, matrix, i, r, prev_step);

            set_score(dp, matrix, eseq, tscore, min_len, max_len, r, i);
        }
    }

    struct dp_step end_step = {.state = UINT_MAX, .seq_len = UINT_MAX};
    double         fs = final_score(dp, matrix, &end_step, eseq);

    if (path)
        viterbi_path(dp, matrix, path, &end_step, eseq);

    return fs;
}

static void viterbi_path(struct imm_dp const* dp, struct dp_matrix const* matrix,
                         struct imm_path* path, struct dp_step const* end_step,
                         struct eseq const* eseq)
{
    unsigned              row = eseq_length(eseq);
    struct dp_step const* step = end_step;

    while (step->seq_len != UINT_MAX) {
        struct imm_state const* state = mstate_get_state(dp->mstates[step->state]);
        struct imm_step*        new_step = imm_step_create(state, step->seq_len);
        imm_path_prepend(path, new_step);
        row -= step->seq_len;
        step = dp_matrix_get_prev_step(matrix, row, step->state);
    }
}

static double best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                               unsigned target_state, unsigned row, struct dp_step* prev_step)
{
    double score = imm_lprob_zero();
    prev_step->state = UINT_MAX;
    prev_step->seq_len = UINT_MAX;

    for (unsigned i = 0; i < dp_trans_table_ntrans(dp->trans_table, target_state); ++i) {

        unsigned source_state = dp_trans_table_source_state(dp->trans_table, target_state, i);
        unsigned min_seq = dp_state_table_min_seq(dp->state_table, source_state);

        if (UNLIKELY(row < min_seq) || (min_seq == 0 && source_state > target_state))
            continue;

        unsigned max_seq = MIN(dp_state_table_max_seq(dp->state_table, source_state), row);
        for (unsigned len = min_seq; len <= max_seq; ++len) {

            struct dp_step const step = {.state = source_state, .seq_len = len};
            unsigned             prev_row = row - len;

            double v0 = dp_matrix_get_score(matrix, prev_row, step);
            double v1 = dp_trans_table_score(dp->trans_table, target_state, i);
            double v = v0 + v1;

            if (v > score) {
                score = v;
                prev_step->state = source_state;
                prev_step->seq_len = len;
            }
        }
    }

#if 0
    if (UNLIKELY(row == 0)) {
        double start = dp_states_start_lprob(dp->states, target_state);
        if (start > score) {
            score = start;
            /* prev_step->state = UINT_MAX; */
            /* prev_step->seq_len = UINT_MAX; */
        }
    }
    /* else if (prev_step->state == UINT_MAX) */
    /*     return imm_lprob_invalid(); */
#endif

    return score;
}

static double final_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                          struct dp_step* end_step, struct eseq const* eseq)
{
    double   score = imm_lprob_zero();
    unsigned end_state = dp_state_table_end_state(dp->state_table);

    end_step->state = UINT_MAX;
    end_step->seq_len = UINT_MAX;
    struct dp_step step = {.state = end_state};

    unsigned length = eseq_length(eseq);

    for (unsigned len = MIN(dp_state_table_max_seq(dp->state_table, end_state), length);;
         --len) {

        step.seq_len = len;
        double s = dp_matrix_get_score(matrix, length - len, step);
        if (s > score) {
            score = s;
            end_step->state = end_state;
            end_step->seq_len = len;
        }

        if (dp_state_table_min_seq(dp->state_table, end_state) == len)
            break;
    }
    if (end_step->state == UINT_MAX)
        return imm_lprob_invalid();

    return score;
}

static unsigned min_seq(struct mstate const* const* mstates, uint32_t nstates)
{
    unsigned min = imm_state_min_seq(mstate_get_state(mstates[0]));
    for (uint32_t i = 1; i < nstates; ++i)
        min = MIN(min, imm_state_min_seq(mstate_get_state(mstates[i])));

    return min;
}

static unsigned max_seq(struct mstate const* const* mstates, uint32_t nstates)
{
    unsigned max = imm_state_max_seq(mstate_get_state(mstates[0]));
    for (uint32_t i = 1; i < nstates; ++i)
        max = MAX(max, imm_state_max_seq(mstate_get_state(mstates[i])));

    return max;
}
