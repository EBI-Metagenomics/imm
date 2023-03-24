#include "imm/viterbi.h"
#include "assume.h"
#include "imm/btrans.h"
#include "imm/cell.h"
#include "imm/dp.h"
#include "imm/float.h"
#include "imm/matrix.h"
#include "imm/state_table.h"
#include "imm/trans_table.h"
#include "minmax.h"
#include "span.h"
#include "task.h"

unsigned imm_viterbi_nstates(struct imm_viterbi const *x)
{
    return x->dp->state_table.nstates;
}

unsigned imm_viterbi_ntrans(struct imm_viterbi const *x, unsigned dst)
{
    return imm_trans_table_ntrans(&x->dp->trans_table, dst);
}

unsigned imm_viterbi_source(struct imm_viterbi const *x, unsigned dst,
                            unsigned trans)
{
    return imm_trans_table_source_state(&x->dp->trans_table, dst, trans);
}

struct imm_range imm_viterbi_range(struct imm_viterbi const *x, unsigned state)
{
    return imm_state_table_range(&x->dp->state_table, state);
}

imm_float imm_viterbi_get_score(struct imm_viterbi const *x, unsigned row,
                                unsigned state, unsigned len)
{
    struct imm_cell cell = imm_cell_init(row - len, state, len);
    return imm_matrix_get_score(&x->task->matrix, cell);
}

void imm_viterbi_set_score(struct imm_viterbi const *x, struct imm_cell cell,
                           imm_float score)
{
    imm_matrix_set_score(&x->task->matrix, cell, score);
}

imm_float imm_viterbi_trans_score(struct imm_viterbi const *x, unsigned dst,
                                  unsigned trans)
{
    return imm_trans_table_score(&x->dp->trans_table, dst, trans);
}

unsigned imm_viterbi_start_state(struct imm_viterbi const *x)
{
    return x->dp->state_table.start.state;
}

imm_float imm_viterbi_start_lprob(struct imm_viterbi const *x)
{
    return x->dp->state_table.start.lprob;
}

imm_float imm_viterbi_emission(struct imm_viterbi const *x, unsigned row,
                               unsigned state, unsigned len, unsigned min_len)
{
    unsigned code = imm_eseq_get(&x->task->eseq, row, len, min_len);
    return imm_emis_score(&x->dp->emis, state, code);
}
