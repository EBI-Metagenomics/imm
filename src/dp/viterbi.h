#ifndef DP_VITERBI_H
#define DP_VITERBI_H

struct imm_dp;
struct imm_task;
struct unsafe_pair;

void viterbi_unsafe(struct imm_dp const *dp, struct imm_task *task,
                    unsigned const start_row, unsigned const stop_row,
                    unsigned seqlen, unsigned unsafe_state);
void viterbi_unsafe_nopath(struct imm_dp const *dp, struct imm_task *task,
                           unsigned const start_row, unsigned const stop_row,
                           unsigned seqlen, unsigned unsafe_state);

void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state);
void viterbi_safe_future_nopath(struct imm_dp const *dp, struct imm_task *task,
                                unsigned const start_row,
                                unsigned const stop_row, unsigned unsafe_state);

void viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                  unsigned const start_row, unsigned const stop_row,
                  unsigned unsafe_state);

void viterbi_safe_nopath(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         unsigned unsafe_state);

void viterbi_row0_safe(struct imm_dp const *dp, struct imm_task *task);

void viterbi_row0(struct imm_dp const *dp, struct imm_task *task,
                  unsigned remain);

#endif
