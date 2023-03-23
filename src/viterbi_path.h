#ifndef VITERBI_PATH_H
#define VITERBI_PATH_H

struct imm_viterbi;
struct imm_range;

void viterbi_path_unsafe(struct imm_viterbi const *, struct imm_range const *,
                         unsigned len);
void viterbi_path_safe_past(struct imm_viterbi const *,
                            struct imm_range const *);
void viterbi_path_row0(struct imm_viterbi const *, unsigned remain);
void viterbi_path_safe_future(struct imm_viterbi const *,
                              struct imm_range const *);
void viterbi_path_safe(struct imm_viterbi const *, struct imm_range const *);
void viterbi_path_safe_row0(struct imm_viterbi const *);

#endif
