#ifndef VITERBI_NOPATH_H
#define VITERBI_NOPATH_H

struct imm_viterbi;
struct imm_range;

void viterbi_nopath(struct imm_viterbi const *, struct imm_range const *);
void viterbi_nopath_row0(struct imm_viterbi const *);
void viterbi_nopath_safe_future(struct imm_viterbi const *,
                                struct imm_range const *);
void viterbi_nopath_safe(struct imm_viterbi const *, struct imm_range const *);
void viterbi_nopath_safe_row0(struct imm_viterbi const *);

#endif
