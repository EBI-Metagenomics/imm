#include "imm/seq.h"
#include "bug.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/report.h"
#include "subseq.h"
#include <limits.h>
#include <string.h>

/**
 *
 * Sequence of characters. No need for null-terminated string.
 */
struct imm_seq
{
    struct imm_abc const* abc;
    char const*           string;
    unsigned              length;
};

struct subseq
{
    struct imm_seq const* seq;
    struct imm_seq        subseq;
};

struct imm_seq const* imm_seq_create(char const* seq, struct imm_abc const* abc)
{
    size_t length = strlen(seq);
    if (length > UINT_MAX) {
        imm_error("sequence is too long");
        return NULL;
    }

    for (unsigned i = 0; i < length; ++i) {
        if (!imm_abc_has_symbol(abc, seq[i]) && seq[i] != imm_abc_any_symbol(abc)) {
            imm_error("symbol not found in the alphabet");
            return NULL;
        }
    }

    struct imm_seq* ret = malloc(sizeof(struct imm_seq));
    ret->abc = abc;
    ret->string = strdup(seq);
    ret->length = (unsigned)length;
    return ret;
}

struct imm_abc const* imm_seq_get_abc(struct imm_seq const* seq) { return seq->abc; }

unsigned imm_seq_length(struct imm_seq const* seq) { return seq->length; }

char const* imm_seq_string(struct imm_seq const* seq) { return seq->string; }

struct imm_seq const* imm_seq_duplicate(struct imm_seq const* seq)
{
    struct imm_seq* ret = malloc(sizeof(struct imm_seq));
    ret->abc = seq->abc;
    ret->string = strndup(seq->string, seq->length);
    ret->length = seq->length;
    return ret;
}

void imm_seq_destroy(struct imm_seq const* seq)
{
    free_c(seq->string);
    free_c(seq);
}

struct subseq* subseq_create(struct imm_seq const* seq)
{
    struct subseq* subseq = malloc(sizeof(struct subseq));
    subseq->seq = seq;
    subseq->subseq.abc = seq->abc;
    return subseq;
}

void subseq_set(struct subseq* subseq, unsigned start, unsigned length)
{
    BUG(start + length > subseq->seq->length);
    subseq->subseq.string = subseq->seq->string + start;
    subseq->subseq.length = length;
}

struct imm_seq const* subseq_cast(struct subseq const* subseq) { return &subseq->subseq; }

void subseq_destroy(struct subseq const* subseq) { free_c(subseq); }
