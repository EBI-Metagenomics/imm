#ifndef DP_VITERBI_H
#define DP_VITERBI_H

struct imm_dp;
struct imm_task;
struct unsafe_pair;

void viterbi_unsafe(struct imm_dp const *dp, struct imm_task *task,
                    unsigned const start_row, unsigned const stop_row,
                    unsigned seqlen, struct unsafe_pair *upair);

void viterbi_safe_future(struct imm_dp const *dp, struct imm_task *task,
                         unsigned const start_row, unsigned const stop_row,
                         struct unsafe_pair *upair);

void viterbi_safe(struct imm_dp const *dp, struct imm_task *task,
                  unsigned const start_row, unsigned const stop_row,
                  struct unsafe_pair *upair);

void viterbi_row0_safe(struct imm_dp const *dp, struct imm_task *task);

void viterbi_row0(struct imm_dp const *dp, struct imm_task *task,
                  unsigned remain);

#endif
