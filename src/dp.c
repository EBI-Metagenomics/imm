#include "dp.h"
#include "compiler.h"
#include "dp_emission.h"
#include "dp_matrix.h"
#include "dp_state_table.h"
#include "dp_step.h"
#include "dp_trans_table.h"
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
#include "io.h"
#include "min.h"
#include "mstate.h"
#include "mtrans.h"
#include "mtrans_table.h"
#include "seq_code.h"
#include "state_idx.h"
#include "thread.h"
#include <limits.h>
#include <stdio.h>

struct task
{
    struct dp_matrix* matrix;
    struct eseq*      eseq;
};

struct imm_dp
{
    struct mstate const* const*  mstates;
    struct seq_code const*       seq_code;
    struct dp_emission const*    emission;
    struct dp_trans_table const* trans_table;
    struct dp_state_table const* state_table;
    unsigned                     ntasks;
    struct task*                 tasks;
};

static double  best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                                uint32_t target_state, uint32_t row, struct dp_step* prev_step);
static void    create_tasks(struct imm_dp* dp);
static double  final_score(struct imm_dp const* dp, struct task* task, struct dp_step* end_step);
static uint8_t max_seq(struct mstate const* const* mstates, uint32_t nstates);
static uint8_t min_seq(struct mstate const* const* mstates, uint32_t nstates);
static inline void set_score(struct imm_dp const* dp, struct task* task, double trans_score,
                             uint32_t min_len, uint32_t max_len, uint32_t row, uint32_t state);
static void        task_create(struct task* task, struct dp_state_table const* state_table,
                               struct seq_code const* seq_code);
static void        task_destroy(struct task* task);
static inline void task_setup(struct task* task, struct imm_seq const* seq);
static double      viterbi(struct imm_dp const* dp, struct task* task, struct imm_path* path);
static void viterbi_path(struct imm_dp const* dp, struct task const* task, struct imm_path* path,
                         struct dp_step const* end_step);

void imm_dp_destroy(struct imm_dp const* dp)
{
    free_c(dp->mstates);
    seq_code_destroy(dp->seq_code);
    dp_emission_destroy(dp->emission);
    dp_trans_table_destroy(dp->trans_table);
    dp_state_table_destroy(dp->state_table);

    for (unsigned i = 0; i < dp->ntasks; ++i) {
        if (dp->tasks[i].matrix)
            task_destroy(dp->tasks + i);
    }

    free_c(dp->tasks);
    free_c(dp);
}

struct imm_results const* imm_dp_viterbi(struct imm_dp const* dp, struct imm_seq const* seq,
                                         unsigned window_length)
{
    if (seq_code_abc(dp->seq_code) != imm_seq_get_abc(seq)) {
        imm_error("dp and seq must have the same alphabet");
        return NULL;
    }

    struct imm_state const* end_state = mstate_get_state(dp->mstates[dp->state_table->end_state]);
    if (imm_seq_length(seq) < imm_state_min_seq(end_state)) {
        imm_error("sequence is shorter than end_state's lower bound");
        return NULL;
    }

    if (window_length == 0)
        window_length = imm_seq_length(seq);

    struct imm_window const* window = imm_window_create(seq, window_length);
    unsigned const           nwindows = imm_window_size(window);
    struct imm_results*      results = imm_results_create(seq, nwindows);

    _Pragma("omp parallel if(nwindows > 1)")
    {
        if (!dp->tasks[thread_id()].matrix)
            task_create(dp->tasks + thread_id(), dp->state_table, dp->seq_code);

#pragma omp for
        for (unsigned i = 0; i < nwindows; ++i) {
            struct imm_subseq const subseq = imm_window_get(window, i);
            struct task*            task = dp->tasks + thread_id();

            task_setup(task, imm_subseq_cast(&subseq));

            struct imm_path* path = imm_path_create();
            double           score = viterbi(dp, task, path);
            imm_results_set(results, i, subseq, path, score);
        }
    }
    imm_window_destroy(window);

    return results;
}

void __imm_dp_create_from_io(struct imm_io* io)
{
    struct imm_dp* dp = malloc(sizeof(*dp));

    dp->mstates = (struct mstate const**)io->mstates;
    dp->seq_code = io->seq_code;
    dp->emission = io->emission;
    dp->trans_table = io->trans_table;
    dp->state_table = io->state_table;

    create_tasks(dp);

    io->dp = dp;
}

struct imm_dp const* dp_create(struct imm_abc const* abc, struct mstate const** mstates,
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

    create_tasks(dp);

    return dp;
}

struct dp_emission const* dp_get_emission(struct imm_dp const* dp) { return dp->emission; }

struct mstate const* const* dp_get_mstates(struct imm_dp const* dp) { return dp->mstates; }

struct seq_code const* dp_get_seq_code(struct imm_dp const* dp) { return dp->seq_code; }

struct dp_state_table const* dp_get_state_table(struct imm_dp const* dp) { return dp->state_table; }

struct dp_trans_table const* dp_get_trans_table(struct imm_dp const* dp) { return dp->trans_table; }

static double best_trans_score(struct imm_dp const* dp, struct dp_matrix const* matrix,
                               uint32_t target_state, uint32_t row, struct dp_step* prev_step)
{
    double score = imm_lprob_zero();
    *prev_step = dp_step_invalid();

    for (uint32_t i = 0; i < dp_trans_table_ntrans(dp->trans_table, target_state); ++i) {

        uint32_t source_state = dp_trans_table_source_state(dp->trans_table, target_state, i);
        uint32_t min_seq = dp_state_table_min_seq(dp->state_table, source_state);

        if (UNLIKELY(row < min_seq) || (min_seq == 0 && source_state > target_state))
            continue;

        uint32_t max_seq = MIN(dp_state_table_max_seq(dp->state_table, source_state), row);
        for (uint32_t len = min_seq; len <= max_seq; ++len) {

            struct dp_step step = {.state = source_state, .seq_len = len};
            uint32_t       prev_row = row - len;

            double v0 = dp_matrix_get_score(matrix, prev_row, step);
            double v1 = dp_trans_table_score(dp->trans_table, target_state, i);
            double v = v0 + v1;

            if (v > score) {
                score = v;
                *prev_step = step;
            }
        }
    }

    return score;
}

static void create_tasks(struct imm_dp* dp)
{
    dp->ntasks = thread_max_size();
    dp->tasks = malloc(sizeof(*dp->tasks) * dp->ntasks);

    for (unsigned i = 0; i < dp->ntasks; ++i) {
        dp->tasks[i].matrix = NULL;
        dp->tasks[i].eseq = NULL;
    }
}

static double final_score(struct imm_dp const* dp, struct task* task, struct dp_step* end_step)
{
    double   score = imm_lprob_zero();
    uint32_t end_state = dp_state_table_end_state(dp->state_table);

    *end_step = dp_step_invalid();
    struct dp_step step = {.state = end_state};

    unsigned length = eseq_length(task->eseq);

    for (unsigned len = MIN(dp_state_table_max_seq(dp->state_table, end_state), length);; --len) {

        step.seq_len = len;
        double s = dp_matrix_get_score(task->matrix, length - len, step);
        if (s > score) {
            score = s;
            end_step->state = end_state;
            end_step->seq_len = len;
        }

        if (dp_state_table_min_seq(dp->state_table, end_state) == len)
            break;
    }
    if (end_step->state == UINT32_MAX)
        return imm_lprob_invalid();

    return score;
}

static uint8_t max_seq(struct mstate const* const* mstates, uint32_t nstates)
{
    uint8_t max = imm_state_max_seq(mstate_get_state(mstates[0]));
    for (uint32_t i = 1; i < nstates; ++i)
        max = MAX(max, imm_state_max_seq(mstate_get_state(mstates[i])));

    return max;
}

static uint8_t min_seq(struct mstate const* const* mstates, uint32_t nstates)
{
    uint8_t min = imm_state_min_seq(mstate_get_state(mstates[0]));
    for (uint32_t i = 1; i < nstates; ++i)
        min = MIN(min, imm_state_min_seq(mstate_get_state(mstates[i])));

    return min;
}

static inline void set_score(struct imm_dp const* dp, struct task* task, double trans_score,
                             uint32_t min_len, uint32_t max_len, uint32_t row, uint32_t state)
{
    for (uint32_t len = min_len; len <= max_len; ++len) {
        struct dp_step step = {.state = state, .seq_len = len};

        unsigned seq_code = eseq_get(task->eseq, row, len);
        seq_code -= seq_code_offset(dp->seq_code, min_len);

        double score = trans_score + dp_emission_score(dp->emission, state, seq_code);
        dp_matrix_set_score(task->matrix, row, step, score);
    }
}

static void task_create(struct task* task, struct dp_state_table const* state_table,
                        struct seq_code const* seq_code)
{
    task->matrix = dp_matrix_create(state_table);
    task->eseq = seq_code_create_eseq(seq_code);
}

static void task_destroy(struct task* task)
{
    dp_matrix_destroy(task->matrix);
    eseq_destroy(task->eseq);
}

static inline void task_setup(struct task* task, struct imm_seq const* seq)
{
    eseq_setup(task->eseq, seq);
    dp_matrix_setup(task->matrix, task->eseq);
}

static double viterbi(struct imm_dp const* dp, struct task* task, struct imm_path* path)
{
    unsigned remain = eseq_length(task->eseq);

    for (unsigned r = 0; r <= eseq_length(task->eseq); ++r) {

        for (uint32_t i = 0; i < dp_state_table_nstates(dp->state_table); ++i) {

            struct dp_step* prev_step = dp_matrix_get_prev_step(task->matrix, r, i);
            double          tscore = best_trans_score(dp, task->matrix, i, r, prev_step);
            if (r == 0)
                tscore = MAX(dp_state_table_start_lprob(dp->state_table, i), tscore);

            unsigned min_len = dp_state_table_min_seq(dp->state_table, i);
            unsigned max_len = MIN(dp_state_table_max_seq(dp->state_table, i), remain);
            set_score(dp, task, tscore, min_len, max_len, r, i);
        }
        --remain;
    }

    struct dp_step end_step = dp_step_invalid();
    double const   score = final_score(dp, task, &end_step);

    if (path)
        viterbi_path(dp, task, path, &end_step);

    return score;
}

static void viterbi_path(struct imm_dp const* dp, struct task const* task, struct imm_path* path,
                         struct dp_step const* end_step)
{
    unsigned              row = eseq_length(task->eseq);
    struct dp_step const* step = end_step;

    while (step->seq_len != UINT_MAX) {
        struct imm_state const* state = mstate_get_state(dp->mstates[step->state]);
        struct imm_step*        new_step = imm_step_create(state, step->seq_len);
        imm_path_prepend(path, new_step);
        row -= step->seq_len;
        step = dp_matrix_get_prev_step_c(task->matrix, row, step->state);
    }
}
