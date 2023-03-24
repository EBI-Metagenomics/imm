#include "viterbi_generic.h"
#include "cpath.h"
#include "imm/lprob.h"
#include "imm/viterbi.h"
#include "minmax.h"
#include "span.h"
#include "task.h"

struct bt2
{
    imm_float score;
    unsigned prev_state;
    unsigned trans;
    unsigned len;
};

#define INIT_BT2                                                               \
    (struct bt2)                                                               \
    {                                                                          \
        imm_lprob_zero(), IMM_STATE_NULL_IDX, IMM_STATE_NULL_SEQLEN,           \
            UINT16_MAX                                                         \
    }

static void set_trans(struct bt2 *x, imm_float score, unsigned prev_state,
                      unsigned trans, unsigned len)
{
    x->score = score;
    x->prev_state = prev_state;
    x->trans = trans;
    x->len = len;
}

static inline void set_cpath(struct cpath *path, struct bt2 const *bt,
                             unsigned r, unsigned i)
{
    if (bt->prev_state != IMM_STATE_NULL_IDX)
    {
        path_set_trans(path, r, i, bt->trans);
        path_set_seqlen(path, r, i, bt->len);
        assert(path_trans(path, r, i) == bt->trans);
        assert(path_seqlen(path, r, i) == bt->len);
    }
    else
    {
        path_invalidate(path, r, i);
        assert(!path_valid(path, r, i));
    }
}

static void generic(struct imm_viterbi const *x, unsigned row, unsigned dst,
                    unsigned remain)
{
    struct bt2 bt = INIT_BT2;
    for (unsigned t = 0; t < imm_viterbi_ntrans(x, dst); ++t)
    {
        unsigned src = imm_viterbi_source(x, dst, t);
        struct imm_range src_range = imm_viterbi_range(x, src);
        src_range.b = MIN(src_range.b, row + 1);

        for (unsigned len = src_range.a; len < src_range.b; ++len)
        {
            imm_float v0 = imm_viterbi_get_score(x, row, src, len);
            imm_float v1 = imm_viterbi_trans_score(x, dst, t);
            imm_float v = v0 + v1;
            unsigned step_len = len - src_range.a;

            if (bt.score < v) set_trans(&bt, v, src, t, step_len);
        }
    }
    if (row == 0 && imm_viterbi_start_state(x) == dst)
        bt.score = MAX(imm_viterbi_start_lprob(x), bt.score);

    set_cpath(&x->task->path, &bt, row, dst);

    struct imm_range dst_range = imm_viterbi_range(x, dst);
    dst_range.b = MIN(dst_range.b, remain + 1);

    for (unsigned len = dst_range.a; len < dst_range.b; ++len)
    {
        unsigned min = dst_range.a;
        imm_float v = imm_viterbi_emission(x, row, dst, len, min);
        imm_float total = bt.score + v;
        imm_viterbi_set_score(x, imm_cell_init(row, dst, len), total);
    }
}

void viterbi_generic(struct imm_viterbi const *x, struct imm_range const *range)
{
    for (unsigned row = range->a; row < range->b; ++row)
    {
        generic(x, row, x->unsafe_state, range->b - 1 - row);
        for (unsigned dst = 0; dst < imm_viterbi_nstates(x); ++dst)
            generic(x, row, dst, range->b - 1 - row);
    }
}
