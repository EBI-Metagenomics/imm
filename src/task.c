#include "imm/task.h"
#include "dp/imm_dp.h"
#include "imm/seq.h"
#include "task.h"

struct imm_task *imm_task_new(struct imm_dp const *dp)
{
    struct imm_task *task = xmalloc(sizeof(*task));
    matrix_init(&task->matrix, &dp->state_table);
    path_init(&task->path, &dp->state_table, &dp->trans_table);
    eseq_init(&task->eseq, &dp->code);
    task->seq = NULL;
    return task;
}

int imm_task_reset(struct imm_task *task, struct imm_dp const *dp)
{
    matrix_reset(&task->matrix, &dp->state_table);
    path_reset(&task->path, &dp->state_table, &dp->trans_table);
    eseq_reset(&task->eseq, &dp->code);
    task->seq = NULL;
    return IMM_SUCCESS;
}

int imm_task_setup(struct imm_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    eseq_setup(&task->eseq, seq);
    path_setup(&task->path, imm_seq_size(seq) + 1);
    return IMM_SUCCESS;
}

void imm_task_del(struct imm_task const *task)
{
    if (task)
    {
        matrix_del(&task->matrix);
        path_del(&task->path);
        eseq_del(&task->eseq);
        free((void *)task);
    }
}
