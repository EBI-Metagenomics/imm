#include "dp_task.h"
#include "dp.h"
#include "dp_matrix.h"
#include "imm/imm.h"
#include <stdlib.h>

struct imm_dp_task* imm_dp_task_create(struct imm_dp const* dp)
{
    struct imm_dp_task* task = malloc(sizeof(*task));
    task->matrix = dp_matrix_create(dp->state_table);
    task->eseq = seq_code_create_eseq(dp->seq_code);
    task->seq = NULL;
    return task;
}

void imm_dp_task_setup(struct imm_dp_task* task, struct imm_seq const* seq)
{
    task->seq = seq;
    eseq_setup(task->eseq, seq);
    dp_matrix_setup(task->matrix, task->eseq);
}

void imm_dp_task_destroy(struct imm_dp_task const* task)
{
    dp_matrix_destroy(task->matrix);
    eseq_destroy(task->eseq);
    free_c(task);
}
