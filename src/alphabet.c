#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include "report.h"
#include <limits.h>
#include <stdlib.h>

int check_symbols_length(const char *symbols);

NHMM_API struct nhmm_alphabet *nhmm_alphabet_create(const char *symbols)
{
    if (check_symbols_length(symbols))
        return NULL;

    struct nhmm_alphabet *a = malloc(sizeof(struct nhmm_alphabet));

    rs_init_w(&a->symbols, symbols);

    for (int i = 0; i <= NHMM_SYMBOL_ID_MAX; ++i)
        a->symbol_idx[i] = -1;

    const char *ids = rs_data_c(&a->symbols);
    for (int idx = 0; idx < (int)rs_len(&a->symbols); ++idx) {
        if (check_symbol_id_range(ids[idx])) {
            rs_free(&a->symbols);
            free(a);
            return NULL;
        }

        a->symbol_idx[(size_t)ids[idx]] = idx;
    }

    return a;
}

NHMM_API size_t nhmm_alphabet_length(const struct nhmm_alphabet *alphabet)
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

int check_symbols_length(const char *symbols)
{
    if (strlen(symbols) > INT_MAX) {
        error("symbols length is greater than %d", INT_MAX);
        return 1;
    }
    return 0;
}
