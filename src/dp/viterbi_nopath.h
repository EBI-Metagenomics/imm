#ifndef DP_VITERBI_NOPATH_H
#define DP_VITERBI_NOPATH_H

struct imm_dp;
struct imm_task;

void viterbi_unsafe_nopath(struct imm_dp const *dp, struct imm_task *task,
                           unsigned const start_row, unsigned const stop_row,
                           unsigned seqlen, unsigned unsafe_state);

void viterbi_safe_future_nopath(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const start_row,
                                unsigned const stop_row, unsigned unsafe_state);

void viterbi_safe_nopath(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state);

#endif
