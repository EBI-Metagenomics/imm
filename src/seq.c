#include "imm/seq.h"
#include "free.h"
#include "imm/abc.h"
#include "imm/report.h"
#include <limits.h>
#include <string.h>

struct imm_seq const* imm_seq_clone(struct imm_seq const* seq)
{
    struct imm_seq* new_seq = malloc(sizeof(*new_seq));
    new_seq->abc = seq->abc;
    new_seq->string = strndup(seq->string, seq->length);
    new_seq->length = seq->length;
    return new_seq;
}

struct imm_seq const* imm_seq_create(char const* string, struct imm_abc const* abc)
{
    size_t length = strlen(string);
    if (length > IMM_SEQ_MAX_LEN) {
        imm_error("sequence is too long");
        return NULL;
    }

    for (uint_fast32_t i = 0; i < length; ++i) {
        if (!imm_abc_has_symbol(abc, string[i]) && string[i] != imm_abc_any_symbol(abc)) {
            imm_error("symbol not found in the alphabet");
            return NULL;
        }
    }

    struct imm_seq* seq = malloc(sizeof(*seq));
    seq->abc = abc;
    seq->string = strdup(string);
    seq->length = (uint32_t)length;
    return seq;
}

void imm_seq_destroy(struct imm_seq const* seq)
{
    free_c(seq->string);
    free_c(seq);
}
