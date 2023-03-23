#ifndef SET_SCORE_H
#define SET_SCORE_H

#include "assume.h"
#include "imm/cell.h"
#include "imm/dp.h"
#include "imm/emis.h"
#include "imm/matrix.h"
#include "span.h"
#include "task.h"

static inline void set_score(struct imm_dp const *dp, struct imm_task *task,
                             imm_float trans_score, unsigned row,
                             unsigned state, struct span span)
{
    assume(span.max <= IMM_STATE_MAX_SEQLEN);
    for (unsigned i = span.min; i <= span.max; ++i)
    {
        unsigned seq_code = imm_eseq_get(&task->eseq, row, i, span.min);
        imm_float score =
            trans_score + imm_emis_score(&dp->emis, state, seq_code);
        imm_matrix_set_score(&task->matrix, imm_cell_init(row, state, i),
                             score);
    }
}

#endif
