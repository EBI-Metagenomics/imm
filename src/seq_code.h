#ifndef IMM_SEQ_CODE_H
#define IMM_SEQ_CODE_H

struct imm_abc;
struct imm_seq;

struct seq_code const* seq_code_create(struct imm_abc const* abc, unsigned min_seq,
                                       unsigned max_seq);
unsigned seq_code_get(struct seq_code const* seq_code, struct imm_seq const* seq);
void     seq_code_destroy(struct seq_code const* seq_code);

#endif
