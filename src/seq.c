#include "imm/seq.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/report.h"
#include <limits.h>
#include <string.h>

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

struct imm_seq const* imm_seq_clone(struct imm_seq const* seq)
{
    struct imm_seq* ret = malloc(sizeof(struct imm_seq));
    ret->abc = seq->abc;
    ret->string = strndup(seq->string, seq->length);
    ret->length = seq->length;
    return ret;
}

void imm_seq_destroy(struct imm_seq const* seq)
{
    imm_free(seq->string);
    imm_free(seq);
}
