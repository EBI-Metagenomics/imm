#ifndef DP_VITERBI_H
#define DP_VITERBI_H

#include "dp/premise.h"
#include "imm/dp.h"
#include "task.h"

void viterbi3(struct premise premise, struct imm_dp const *dp,
              struct imm_task *task, unsigned start_row, unsigned stop_row);

#endif
