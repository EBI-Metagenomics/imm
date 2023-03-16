#include "dp/unsafe_pair.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"
#include "imm/dp.h"

unsigned find_unsafe_states(struct imm_dp const *dp, struct unsafe_pair *x)
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
                x->src = src;
                x->dst = dst;
                x->trans = t;
            }
        }
    }
    return n;
}
