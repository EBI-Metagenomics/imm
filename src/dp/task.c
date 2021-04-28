#include "dp/task.h"
#include "common/common.h"
#include "dp/dp.h"
#include "dp/eseq.h"
#include "dp/matrix.h"
#include "dp/trans_table.h"
#include "imm/dp_task.h"
#include "imm/seq.h"
#include <stdlib.h>

struct imm_dp_task *imm_dp_task_new(struct imm_dp const *dp)
{
    struct imm_dp_task *task = xmalloc(sizeof(*task));
    matrix_init(&task->matrix, &dp->state_table);
    task->eseq = seq_code_new_eseq(&dp->seq_code);
    path_init(&task->path, &dp->state_table, &dp->trans_table);
    task->eseq = NULL;
    task->seq = NULL;
    return task;
}

int imm_dp_task_setup(struct imm_dp_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    int err = eseq_setup(task->eseq, seq);
    if (err)
        return err;
    return path_setup(&task->path, imm_seq_len(seq) + 1);
}

void imm_dp_task_del(struct imm_dp_task const *task)
{
    matrix_deinit(&task->matrix);
    path_deinit(&task->path);
    eseq_del(task->eseq);
    free((void *)task);
}
