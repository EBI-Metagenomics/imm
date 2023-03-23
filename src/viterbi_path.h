#ifndef VITERBI_PATH_H
#define VITERBI_PATH_H

struct imm_viterbi;
struct imm_range;

void viterbi_unsafe(struct imm_viterbi const *, struct imm_range const *,
                    unsigned len);
void viterbi_safe_past(struct imm_viterbi const *, struct imm_range const *);
void viterbi_safe_future(struct imm_viterbi const *, struct imm_range const *);

void viterbi_safe(struct imm_viterbi const *, struct imm_range const *);

void viterbi_row0_safe(struct imm_viterbi const *);

void viterbi_row0(struct imm_viterbi const *, unsigned remain);

#endif
