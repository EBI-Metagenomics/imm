#include "dp_task.h"
#include "cpath.h"
#include "dp.h"
#include "dp_matrix.h"
#include "dp_trans_table.h"
#include "imm/imm.h"
#include "util/bits.h"
#include "util/math.h"
#include <stdlib.h>

struct imm_dp_task* imm_dp_task_create(struct imm_dp const* dp)
{
    struct imm_dp_task* task = malloc(sizeof(*task));
    task->matrix = dp_matrix_create(dp->state_table);
    task->eseq = seq_code_create_eseq(dp->seq_code);

    cpath_init(&task->cpath, dp_get_state_table(dp), dp_get_trans_table(dp));

    task->seq = NULL;
    return task;
}

void imm_dp_task_setup(struct imm_dp_task* task, struct imm_seq const* seq)
{
    task->seq = seq;
    eseq_setup(task->eseq, seq);
    dp_matrix_setup(task->matrix, task->eseq);
    cpath_setup(&task->cpath, imm_seq_length(seq) + 1);
}

void imm_dp_task_destroy(struct imm_dp_task const* task)
{
    dp_matrix_destroy(task->matrix);
    eseq_destroy(task->eseq);
    cpath_deinit(&task->cpath);
    free_c(task);
}
