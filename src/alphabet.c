#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include <stdlib.h>

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *symbols)
{
    struct nhmm_alphabet *a = malloc(sizeof(struct nhmm_alphabet));
    rs_init_w(&a->symbols, symbols);

    for (int i = 0; i <= NHMM_SYMBOL_ID_LAST; ++i)
        a->symbol_idx[i] = -1;

    for (int idx = 0; idx < rs_len(&a->symbols); ++idx)
        a->symbol_idx[rs_data_c(&a->symbols)[idx]] = idx;

    return a;
}

NHMM_API int nhmm_alphabet_length(const struct nhmm_alphabet *alphabet)
{
    return rs_len(&alphabet->symbols);
}

NHMM_API void nhmm_alphabet_destroy(struct nhmm_alphabet *alphabet)
{
    if (!alphabet)
        return;

    rs_free(&alphabet->symbols);
    free(alphabet);
}
