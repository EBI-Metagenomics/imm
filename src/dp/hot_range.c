#include "dp/hot_range.h"
#include "dp/dp.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"
#include "span.h"

static unsigned find_left(struct imm_dp const *dp, unsigned start,
                          unsigned *left_most)
{
    for (unsigned dst = start; dst < dp->state_table.nstates; ++dst)
    {
        struct span x = state_table_span(&dp->state_table, dst);
        if (x.min == 0 || x.max == 0) continue;

        if (trans_table_ntrans(&dp->trans_table, dst) != 1) continue;

        unsigned src = trans_table_source_state(&dp->trans_table, dst, 0);
        x = state_table_span(&dp->state_table, src);
        if (!(x.min == 0 && x.max == 0)) continue;

        *left_most = src;
        return dst;
    }
    return dp->state_table.nstates;
}

static unsigned find_start(struct imm_dp const *dp, unsigned start,
                           unsigned left_most, struct span span)
{
    for (unsigned dst = start; dst < dp->state_table.nstates; ++dst)
    {
        unsigned ntrans = trans_table_ntrans(&dp->trans_table, dst);
        if (ntrans != 2) continue;

        unsigned src0 = trans_table_source_state(&dp->trans_table, dst, 0);
        unsigned src1 = trans_table_source_state(&dp->trans_table, dst, 1);

        if (src0 != dst - 1) continue;
        if (src1 != left_most) continue;
        struct span x = state_table_span(&dp->state_table, dst);

        if (!(x.min == span.min && x.max == span.max)) continue;
        return dst;
    }
    return dp->state_table.nstates;
}

static unsigned find_total(struct imm_dp const *dp, unsigned start,
                           unsigned left_most, struct span span)
{
    unsigned total = 0;
    for (unsigned dst = start; dst < dp->state_table.nstates; ++dst)
    {
        unsigned ntrans = trans_table_ntrans(&dp->trans_table, dst);
        if (ntrans != 2) break;

        unsigned src0 = trans_table_source_state(&dp->trans_table, dst, 0);
        unsigned src1 = trans_table_source_state(&dp->trans_table, dst, 1);

        if (src0 != dst - 1) break;
        if (src1 != left_most) break;
        struct span x = state_table_span(&dp->state_table, dst);
        if (x.min != span.min || x.max != span.max) break;

        total++;
    }
    return total;
}

void imm_hot_range(struct imm_dp const *dp, struct span span,
                   struct hot_range *hot)
{
    hot->left = 0;
    hot->start = 0;
    hot->total = 0;

    unsigned x = 0;
    unsigned left = 0;
    while ((x = find_left(dp, x, &left)) < dp->state_table.nstates)
    {
        unsigned start = find_start(dp, left + 1, left, span);
        unsigned total = find_total(dp, start + 1, left, span);
        if (total > hot->total)
        {
            hot->left = left;
            hot->start = start;
            hot->total = total;
        }
        x++;
        x = start + total > x ? start + total : x;
    }
}
