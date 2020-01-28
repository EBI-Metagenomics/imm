#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

struct imm_seq;
struct subseq;

struct subseq*        subseq_create(struct imm_seq const* seq);
void                  subseq_set(struct subseq* subseq, unsigned start, unsigned length);
struct imm_seq const* subseq_cast(struct subseq const* subseq);
void                  subseq_destroy(struct subseq const* subseq);

#endif
