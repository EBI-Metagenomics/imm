#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include <stdlib.h>

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *abc)
{
    struct nhmm_alphabet *a = malloc(sizeof(struct nhmm_alphabet));
    a->abc = sdsnew(abc);

    for (int i = 0; i < sizeof(a->idx); ++i)
        a->idx[i] = -1;

    for (int idx = 0; idx < sdslen(a->abc); ++idx)
        a->idx[a->abc[idx]] = idx;

    return a;
}

NHMM_API int nhmm_alphabet_length(const struct nhmm_alphabet *a)
{
    return sdslen(a->abc);
}

NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *a)
{
    if (!a)
        return;

    sdsfree(a->abc);
    free(a);
}
