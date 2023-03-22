#include "dp/viterbi_nopath.h"
#include "dp/hot_range.h"
#include "dp/set_score.h"
#include "dp/state_table.h"
#include "dp/step.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "minmax.h"
#include "range.h"
#include "task.h"

static inline struct span safe_span(struct imm_dp const *dp, unsigned state)
{
    return state_table_span(&dp->state_table, state);
}

static inline struct span rclip(struct span x, unsigned max)
{
    return span_init(x.min, x.max <= max ? x.max : max);
}

static inline struct span unsafe_span(struct imm_dp const *dp, unsigned state,
                                      unsigned max)
{
    return rclip(state_table_span(&dp->state_table, state), max);
}

static inline void _viti_safe_nopath(struct imm_dp const *, struct imm_task *,
                                     unsigned r, unsigned i);
static inline void _viti_safe_hot_nopath(struct imm_dp const *,
                                         struct imm_task *, unsigned row,
                                         unsigned dst,
                                         struct hot_range const *);

void viterbi_nopath_unsafe(struct imm_dp const *dp, struct imm_task *task,
                           struct imm_range const *range, unsigned len,
                           unsigned unsafe_state)
{
    struct imm_dp_step step = imm_dp_step_init(dp, &task->matrix);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        if ((r > 0 && r < len))
        {
            unsigned i = unsafe_state;
            imm_float score = imm_dp_step_score_ge1(&step, i, r);
            unsigned rem = range->b - r - 1;
            set_score(dp, task, score, r, i, unsafe_span(dp, i, rem));
        }

        for (unsigned i = 0; i < imm_dp_nstates(dp); ++i)
        {
            imm_float score = imm_dp_step_score(&step, i, r);
            unsigned rem = range->b - r - 1;
            set_score(dp, task, score, r, i, unsafe_span(dp, i, rem));
        }
    }
}

void viterbi_nopath_safe_future(struct imm_dp const *dp, struct imm_task *task,
                                struct imm_range const *range,
                                unsigned unsafe_state)
{
    assert(range->a > 0);

    struct imm_dp_step step = imm_dp_step_init(dp, &task->matrix);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        unsigned i = unsafe_state;
        imm_float score = imm_dp_step_score_ge1(&step, i, r);
        set_score(dp, task, score, r, i, safe_span(dp, i));

        for (unsigned i = 0; i < imm_dp_nstates(dp); ++i)
        {
            imm_float score = imm_dp_step_score(&step, i, r);
            set_score(dp, task, score, r, i, safe_span(dp, i));
        }
    }
}

void viterbi_nopath_safe(struct imm_dp const *dp, struct imm_task *task,
                         struct imm_range const *range, unsigned unsafe_state)
{
    struct hot_range hot = {0};
    imm_hot_range(dp, (struct span){1, 1}, &hot);
    assume(range->a > 0);

    struct imm_dp_step step = imm_dp_step_init(dp, &task->matrix);

    for (unsigned r = range->a; r < range->b; ++r)
    {
        unsigned state = unsafe_state;
        imm_float score = imm_dp_step_score_safe_ge1(&step, state, r);
        set_score(dp, task, score, r, state, safe_span(dp, state));

        if (hot.total < 2)
        {
            for (unsigned i = 0; i < imm_dp_nstates(dp); ++i)
                _viti_safe_nopath(dp, task, r, i);
        }
        else
        {
            for (unsigned i = 0; i < hot.start; ++i)
                _viti_safe_nopath(dp, task, r, i);

            for (unsigned i = hot.start; i < hot.start + hot.total; ++i)
            {
                // _viti_safe_nopath(dp, task, r, i);
                _viti_safe_hot_nopath(dp, task, r, i, &hot);
            }

            for (unsigned i = hot.start + hot.total; i < imm_dp_nstates(dp);
                 ++i)
                _viti_safe_nopath(dp, task, r, i);
        }
    }
}

static inline void _viti_safe_nopath(struct imm_dp const *dp,
                                     struct imm_task *task, unsigned r,
                                     unsigned i)
{
    struct imm_dp_step step = imm_dp_step_init(dp, &task->matrix);
    imm_float score = imm_dp_step_score_safe(&step, i, r);
    set_score(dp, task, score, r, i, safe_span(dp, i));
}

static inline void _viti_safe_hot_nopath(struct imm_dp const *dp,
                                         struct imm_task *task, unsigned row,
                                         unsigned dst,
                                         struct hot_range const *hot)
{
    struct imm_dp_trans_table const *tt = &dp->trans_table;
    struct matrix const *mt = &task->matrix;

    imm_float v0 = matrix_get_score(mt, cell_init(row - 1, dst - 1, 1)) +
                   trans_table_score(tt, dst, 0);
    imm_float v1 = matrix_get_score(mt, cell_init(row, hot->left, 0)) +
                   trans_table_score(tt, dst, 1);

    imm_float score = v0 < v1 ? v1 : v0;
    set_score(dp, task, score, row, dst, span_init(1, 1));
}
