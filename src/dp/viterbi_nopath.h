#ifndef DP_VITERBI_NOPATH_H
#define DP_VITERBI_NOPATH_H

struct imm_dp;
struct imm_task;

void viterbi_nopath_unsafe(struct imm_dp const *dp, struct imm_task *task,
                           unsigned const start_row, unsigned const stop_row,
                           unsigned seqlen, unsigned unsafe_state);

void viterbi_nopath_safe_future(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const start_row,
                                unsigned const stop_row, unsigned unsafe_state);

void viterbi_nopath_safe(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state);

#endif
