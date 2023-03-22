#ifndef DP_VITERBI_NOPATH_H
#define DP_VITERBI_NOPATH_H

struct imm_dp;
struct imm_task;
struct imm_range;

void viterbi_nopath_unsafe(struct imm_dp const *, struct imm_task *,
                           struct imm_range const *, unsigned seqlen,
                           unsigned unsafe_state);

void viterbi_nopath_safe_future(struct imm_dp const *, struct imm_task *,
                                unsigned start_row, unsigned stop_row,
                                unsigned unsafe_state);

void viterbi_nopath_safe(struct imm_dp const *, struct imm_task *,
                         unsigned start_row, unsigned stop_row,
                         unsigned unsafe_state);

#endif
