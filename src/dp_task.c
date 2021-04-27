#include "dp_task.h"
#include "cpath.h"
#include "dp.h"
#include "dp_matrix.h"
#include "dp_trans_table.h"
#include "imm/imm.h"
#include "std.h"
#include <stdlib.h>

struct imm_dp_task *imm_dp_task_create(struct imm_dp const *dp)
{
    struct imm_dp_task *task = xmalloc(sizeof(*task));
    task->matrix = NULL;
    task->eseq = NULL;
    task->seq = NULL;

    if (!(task->matrix = dp_matrix_create(dp->state_table)))
        goto err;

    if (!(task->eseq = seq_code_create_eseq(dp->seq_code)))
        goto err;

    cpath_init(&task->cpath, dp_get_state_table(dp), dp_get_trans_table(dp));

    return task;

err:
    if (task->matrix)
        dp_matrix_destroy(task->matrix);
    if (task->eseq)
        eseq_destroy(task->eseq);
    cpath_deinit(&task->cpath);
    return NULL;
}

int imm_dp_task_setup(struct imm_dp_task *task, struct imm_seq const *seq)
{
    task->seq = seq;
    int err = eseq_setup(task->eseq, seq);
    if (err)
        return err;
    return cpath_setup(&task->cpath, (uint_fast32_t)(imm_seq_length(seq) + 1));
}

void imm_dp_task_destroy(struct imm_dp_task const *task)
{
    dp_matrix_destroy(task->matrix);
    eseq_destroy(task->eseq);
    cpath_deinit(&task->cpath);
    free((void *)task);
}
