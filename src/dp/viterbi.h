#ifndef DP_VITERBI_H
#define DP_VITERBI_H

struct imm_dp;
struct imm_task;

void viterbi_unsafe(struct imm_dp const *, struct imm_task *,
                    unsigned start_row, unsigned stop_row, unsigned seqlen,
                    unsigned unsafe_state);

void viterbi_safe_future(struct imm_dp const *, struct imm_task *,
                         unsigned start_row, unsigned stop_row,
                         unsigned unsafe_state);

void viterbi_safe(struct imm_dp const *, struct imm_task *, unsigned start_row,
                  unsigned stop_row, unsigned unsafe_state);

void viterbi_row0_safe(struct imm_dp const *dp, struct imm_task *task);

void viterbi_row0(struct imm_dp const *dp, struct imm_task *task,
                  unsigned remain);

#endif
