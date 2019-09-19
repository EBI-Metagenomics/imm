#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include <stdlib.h>

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *symbols)
{
    struct nhmm_alphabet *a = malloc(sizeof(struct nhmm_alphabet));
    a->symbols = sdsnew(symbols);

    for (int i = 0; i < sizeof(a->symbol_idx); ++i)
        a->symbol_idx[i] = -1;

    for (int idx = 0; idx < sdslen(a->symbols); ++idx)
        a->symbol_idx[a->symbols[idx]] = idx;

    return a;
}

NHMM_API int nhmm_alphabet_length(const struct nhmm_alphabet *alphabet)
{
    return sdslen(alphabet->symbols);
}

NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *alphabet)
{
    if (!alphabet)
        return;

    sdsfree(alphabet->symbols);
    free(alphabet);
}
