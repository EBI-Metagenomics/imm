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
    if (imm_matrix_init(&task->matrix, &dp->state_table))
    {
        free(task);
        return NULL;
    }
    if (imm_path_init(&task->path, &dp->state_table, &dp->trans_table))
    {
        imm_matrix_del(&task->matrix);
        free(task);
        return NULL;
    }
    imm_eseq_init(&task->eseq, dp->code);
    task->seq = NULL;
    task->save_path = true;
    return task;
}

enum imm_rc imm_task_reset(struct imm_task *task, struct imm_dp const *dp)
{
    enum imm_rc rc = imm_matrix_reset(&task->matrix, &dp->state_table);
    if (rc) return rc;
    /* TODO: memory leak possibility */
    if ((rc = imm__path_reset(&task->path, &dp->state_table, &dp->trans_table)))
        return rc;
    imm_eseq_reset(&task->eseq, dp->code);
    task->seq = NULL;
    return IMM_OK;
}

enum imm_rc imm_task_setup(struct imm_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    enum imm_rc rc = imm_path_setup(&task->path, imm_seq_size(seq) + 1);
    if (rc) return rc;
    return imm_eseq_setup(&task->eseq, seq);
}

void imm_task_set_save_path(struct imm_task *task, bool save)
{
    task->save_path = save;
}

void imm_task_del(struct imm_task const *task)
{
    if (task)
    {
        imm_matrix_del(&task->matrix);
        imm__path_del(&task->path);
        imm_eseq_del(&task->eseq);
        free((void *)task);
    }
}
