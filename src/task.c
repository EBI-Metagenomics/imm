#include "imm/task.h"
#include "error.h"
#include "imm/dp.h"
#include "imm/seq.h"
#include "task.h"
#include <stdio.h>

struct imm_task *imm_task_new(struct imm_dp const *dp)
{
    struct imm_task *task = malloc(sizeof(*task));
    if (!task)
    {
        error(IMM_NOMEM);
        return NULL;
    }
    if (matrix_init(&task->matrix, &dp->state_table))
    {
        free(task);
        return NULL;
    }
    if (path_init(&task->path, &dp->state_table, &dp->trans_table))
    {
        matrix_del(&task->matrix);
        free(task);
        return NULL;
    }
    eseq_init(&task->eseq, dp->code);
    task->seq = NULL;
    return task;
}

enum imm_rc imm_task_reset(struct imm_task *task, struct imm_dp const *dp)
{
    enum imm_rc rc = matrix_reset(&task->matrix, &dp->state_table);
    if (rc) return rc;
    /* TODO: memory leak possibility */
    if ((rc = path_reset(&task->path, &dp->state_table, &dp->trans_table)))
        return rc;
    eseq_reset(&task->eseq, dp->code);
    task->seq = NULL;
    return IMM_OK;
}

enum imm_rc imm_task_setup(struct imm_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    enum imm_rc rc = path_setup(&task->path, imm_seq_size(seq) + 1);
    if (rc) return rc;
    return eseq_setup(&task->eseq, seq);
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
