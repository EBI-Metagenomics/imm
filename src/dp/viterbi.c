#include "dp/viterbi.h"
#include "dp/assume.h"
#include "dp/best_trans.h"
#include "dp/minmax.h"
#include "dp/premise.h"
#include "dp/set_score.h"
#include "dp/state_range.h"
#include "dp/state_table.h"
#include "dp/trans_info.h"
#include "imm/dp.h"
#include "imm/path.h"
#include "imm/prod.h"
#include "range.h"
#include "task.h"
#include <assert.h>

static imm_float final_score(struct imm_dp const *dp, struct imm_task *task,
                             unsigned *state, unsigned *seqlen);
void viterbi4(struct imm_dp const *dp, struct imm_task *task,
              unsigned start_row, unsigned stop_row, unsigned seqlen);
static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len);

static void viterbi_safe_past(struct imm_dp const *dp, struct imm_task *task,
                              unsigned start_row, unsigned stop_row,
                              unsigned seqlen);
static void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                                unsigned start_row, unsigned stop_row,
                                unsigned seqlen);
static void viterbi_very_safe(struct imm_dp const *dp, struct imm_task *task,
                              unsigned start_row, unsigned stop_row,
                              unsigned seqlen);

// Define as:
//   lowest row such that r >= max(max_seq) > 0 for every r >= row.
static struct imm_range find_safe_past(unsigned seqlen)
{
    assert(IMM_STATE_MAX_SEQLEN > 0);
    if (seqlen >= IMM_STATE_MAX_SEQLEN)
        return imm_range_init(IMM_STATE_MAX_SEQLEN, seqlen + 1);
    return imm_range_init(seqlen + 1, seqlen + 1);
}

// Define as:
//   greatest row such that seqlen >= max(max_seq) + r for every r <= row.
static struct imm_range find_safe_future(unsigned seqlen)
{
    if (seqlen >= IMM_STATE_MAX_SEQLEN)
        return imm_range_init(0, seqlen - IMM_STATE_MAX_SEQLEN + 1);
    return imm_range_init(0, 0);
}

static unsigned find_unsafe_states(struct imm_dp const *dp, unsigned *state)
{
    unsigned n = 0;
    for (unsigned dst = 0; dst < dp->state_table.nstates; ++dst)
    {
        for (unsigned t = 0; t < trans_table_ntrans(&dp->trans_table, dst); ++t)
        {
            unsigned src = trans_table_source_state(&dp->trans_table, dst, t);
            struct span span = state_table_span(&dp->state_table, src);
            if (span.min == 0 && dst < src)
            {
                n++;
                *state = dst;
            }
        }
    }
    return n;
}

struct ranges
{
    struct imm_range safe_future;
    struct imm_range safe;
    struct imm_range unsafe;
    struct imm_range safe_past;
};

static void compute_ranges(struct ranges *x, unsigned len)
{
    struct imm_range past = find_safe_past(len);
    struct imm_range future = find_safe_future(len);
    x->safe = imm_range_intersect(past, future);

    struct imm_range tmp = {0};

    imm_range_subtract(future, x->safe, &x->safe_future, &tmp);
    if (imm_range_empty(x->safe_future)) imm_range_swap(&x->safe_future, &tmp);
    assert(imm_range_empty(tmp));

    imm_range_subtract(past, x->safe, &tmp, &x->safe_past);
    if (imm_range_empty(x->safe_past)) imm_range_swap(&x->safe_past, &tmp);
    assert(imm_range_empty(tmp));

    if (imm_range_empty(x->safe))
        imm_range_set(&x->unsafe, x->safe_future.b, x->safe_past.a);
    else
        imm_range_set(&x->unsafe, x->safe.b, x->safe.b);

    assert(imm_range_empty(x->safe) || imm_range_empty(x->unsafe));
}

enum imm_rc viterbi(struct imm_dp const *dp, struct imm_task *task,
                    struct imm_prod *prod)
{

    unsigned unsafe_state = 0;
    unsigned num_unsafe_states = find_unsafe_states(dp, &unsafe_state);

    if (num_unsafe_states > 1) return IMM_TOO_MANY_UNSAFE_STATES;

    unsigned len = eseq_len(&task->eseq);
    struct ranges ranges = {0};
    compute_ranges(&ranges, len);

#if 0
    if (num_unsafe_states == 0)
        printf("No unsafe state found.\n");
    else
        printf("Unsafe state found: %u\n", unsafe_state);
#endif

    viterbi_safe_future(dp, task, ranges.safe_future.a, ranges.safe_future.b,
                        len);

    if (imm_range_empty(ranges.unsafe))
        viterbi_very_safe(dp, task, ranges.safe.a, ranges.safe.b, len);
    else
        viterbi4(dp, task, ranges.unsafe.a, ranges.unsafe.b, len);

    viterbi_safe_past(dp, task, ranges.safe_past.a, ranges.safe_past.b, len);

    unsigned state = 0;
    unsigned seqlen = 0;
    prod->loglik = final_score(dp, task, &state, &seqlen);
    return viterbi_path(dp, task, &prod->path, state, seqlen);
}

static enum imm_rc viterbi_path(struct imm_dp const *dp,
                                struct imm_task const *task,
                                struct imm_path *path, unsigned end_state,
                                unsigned end_seq_len)
{
    unsigned row = eseq_len(&task->eseq);
    unsigned state = end_state;
    unsigned seqlen = end_seq_len;
    bool valid = seqlen != IMM_STATE_NULL_SEQLEN;

    while (valid)
    {
        unsigned id = dp->state_table.ids[state];
        struct imm_step step = imm_step(id, seqlen);
        enum imm_rc rc = imm_path_add(path, step);
        if (rc) return rc;
        row -= seqlen;

        valid = path_valid(&task->path, row, state);
        if (valid)
        {
            unsigned trans = path_trans(&task->path, row, state);
            unsigned len = path_seqlen(&task->path, row, state);
            state = trans_table_source_state(&dp->trans_table, state, trans);
            seqlen = len + state_table_span(&dp->state_table, state).min;
        }
    }
    imm_path_reverse(path);
    return IMM_OK;
}

static imm_float final_score(struct imm_dp const *dp, struct imm_task *task,
                             unsigned *state, unsigned *seqlen)
{
    imm_float score = imm_lprob_zero();
    unsigned end_state = dp->state_table.end_state_idx;

    *state = IMM_STATE_NULL_IDX;
    *seqlen = IMM_STATE_NULL_SEQLEN;

    unsigned length = eseq_len(&task->eseq);
    unsigned max_seq = state_table_span(&dp->state_table, end_state).max;

    for (unsigned len = (unsigned)MIN(max_seq, length);; --len)
    {

        imm_float s =
            matrix_get_score(&task->matrix, length - len, end_state, len);
        if (s > score)
        {
            score = s;
            *state = end_state;
            *seqlen = len;
        }

        if (state_table_span(&dp->state_table, end_state).min == len) break;
    }

    if (*state == IMM_STATE_NULL_IDX) score = imm_lprob_nan();

    return score;
}

void viterbi4(struct imm_dp const *dp, struct imm_task *task,
              unsigned start_row, unsigned stop_row, unsigned seqlen)
{
    assume(stop_row <= seqlen + 1);

    for (unsigned r = start_row; r < stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct trans_info fs = best_trans_score(dp, matrix, i, r);

            path_invalidate(&task->path, r, i);
            path_set_trans(&task->path, r, i, fs.trans);
            path_set_seqlen(&task->path, r, i, fs.seqlen);

            struct span span = state_table_span(&dp->state_table, i);
            if (seqlen < span.min + r) continue;
            if (seqlen < span.max + r) span.max = seqlen - r;

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(&dp->emis, task, r, &range, fs.score);
        }
    }
}

static void viterbi_safe_past(struct imm_dp const *dp, struct imm_task *task,
                              unsigned start_row, unsigned stop_row,
                              unsigned seqlen)
{
    assume(stop_row <= seqlen + 1);

    for (unsigned r = start_row; r < stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct trans_info fs = best_trans_score_safe_past(dp, matrix, i, r);

            path_invalidate(&task->path, r, i);
            path_set_trans(&task->path, r, i, fs.trans);
            path_set_seqlen(&task->path, r, i, fs.seqlen);

            struct span span = state_table_span(&dp->state_table, i);
            if (seqlen < span.min + r) continue;
            if (seqlen <= span.max + r) span.max = seqlen - r;

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(&dp->emis, task, r, &range, fs.score);
        }
    }
}

static void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                                unsigned start_row, unsigned stop_row,
                                unsigned seqlen)
{
    assume(stop_row <= seqlen + 1);

    for (unsigned r = start_row; r < stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct trans_info fs = best_trans_score(dp, matrix, i, r);

            path_invalidate(&task->path, r, i);
            path_set_trans(&task->path, r, i, fs.trans);
            path_set_seqlen(&task->path, r, i, fs.seqlen);

            struct span span = state_table_span(&dp->state_table, i);
            assume(seqlen >= span.max + r);

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(&dp->emis, task, r, &range, fs.score);
        }
    }
}

static void viterbi_very_safe(struct imm_dp const *dp, struct imm_task *task,
                              unsigned start_row, unsigned stop_row,
                              unsigned seqlen)
{
    assume(stop_row <= seqlen + 1);

    for (unsigned r = start_row; r < stop_row; ++r)
    {
        for (unsigned i = 0; i < dp->state_table.nstates; ++i)
        {
            struct matrix const *matrix = &task->matrix;
            struct trans_info fs = best_trans_score_safe_past(dp, matrix, i, r);

            path_invalidate(&task->path, r, i);
            path_set_trans(&task->path, r, i, fs.trans);
            path_set_seqlen(&task->path, r, i, fs.seqlen);

            struct span span = state_table_span(&dp->state_table, i);
            assume(seqlen >= span.max + r);

            struct state_range range = state_range(i, span.min, span.max);
            set_multi_score(&dp->emis, task, r, &range, fs.score);
        }
    }
}
