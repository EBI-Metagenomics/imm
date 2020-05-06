#ifndef IMM_SUBSEQ_H
#define IMM_SUBSEQ_H

#include "imm/export.h"
#include "imm/seq.h"

struct imm_subseq
{
    struct imm_seq const* super;
    struct imm_seq        seq;
};

#define IMM_SUBSEQ(name, seq, start, length)                                                       \
    struct imm_subseq name = imm_subseq_init(&(name), seq, start, length)

static inline struct imm_seq const* imm_subseq_cast(struct imm_subseq const* subseq);
static inline struct imm_subseq     imm_subseq_init(struct imm_subseq*    subseq,
                                                    struct imm_seq const* seq, unsigned start,
                                                    unsigned length);
static inline unsigned              imm_subseq_length(struct imm_subseq const* subseq);
static inline void imm_subseq_set(struct imm_subseq* subseq, unsigned start, unsigned length);
static inline struct imm_subseq imm_subseq_slice(struct imm_seq const* seq, unsigned start,
                                                 unsigned length);
static inline unsigned          imm_subseq_start(struct imm_subseq const* subseq);

static inline struct imm_seq const* imm_subseq_cast(struct imm_subseq const* subseq)
{
    return &subseq->seq;
}

static inline struct imm_subseq imm_subseq_init(struct imm_subseq*    subseq,
                                                struct imm_seq const* seq, unsigned start,
                                                unsigned length)
{
    subseq->super = seq;
    subseq->seq.abc = seq->abc;
    subseq->seq.string = subseq->super->string + start;
    subseq->seq.length = length;
    return *subseq;
}

static inline unsigned imm_subseq_length(struct imm_subseq const* subseq)
{
    return subseq->seq.length;
}

static inline void imm_subseq_set(struct imm_subseq* subseq, unsigned start, unsigned length)
{
    subseq->seq.string = subseq->super->string + start;
    subseq->seq.length = length;
}

static inline struct imm_subseq imm_subseq_slice(struct imm_seq const* seq, unsigned start,
                                                 unsigned length)
{
    IMM_SUBSEQ(subseq, seq, start, length);
    return subseq;
}

static inline unsigned imm_subseq_start(struct imm_subseq const* subseq)
{
    return (unsigned)(subseq->seq.string - subseq->super->string);
}

#endif
