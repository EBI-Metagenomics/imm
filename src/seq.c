#include "imm/seq.h"
#include "free.h"
#include "imm/report.h"
#include <string.h>

struct imm_seq imm_seq_duplicate(struct imm_seq const seq)
{
    return (struct imm_seq){strndup(seq.string, seq.length), seq.length};
}

void imm_seq_destroy(struct imm_seq const seq)
{
    if (!seq.string) {
        imm_error("seq.string should not be NULL");
        return;
    }
    free_c(seq.string);
}
