#include "dp/task.h"
#include "common/common.h"
#include "dp/dp.h"
#include "dp/eseq.h"
#include "dp/imm_dp.h"
#include "imm/dp_task.h"

struct imm_dp_task *imm_dp_task_new(struct imm_dp const *dp)
{
    struct imm_dp_task *task = xmalloc(sizeof(*task));
    matrix_init(&task->matrix, &dp->state_table);
    code_init_eseq(&task->eseq, &dp->code);
    path_init(&task->path, &dp->state_table, &dp->trans_table);
    eseq_init(&task->eseq, &dp->code);
    task->seq = NULL;
    return task;
}

int imm_dp_task_setup(struct imm_dp_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    eseq_setup(&task->eseq, seq);
    path_setup(&task->path, imm_seq_len(seq) + 1);
    return IMM_SUCCESS;
}

void imm_dp_task_del(struct imm_dp_task const *task)
{
    matrix_deinit(&task->matrix);
    path_deinit(&task->path);
    eseq_deinit(&task->eseq);
    free((void *)task);
}
