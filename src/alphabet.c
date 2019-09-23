#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include "report.h"
#include <limits.h>
#include <stdlib.h>

#define NHMM_SYMBOL_ID_MIN 0
#define NHMM_SYMBOL_ID_MAX 127


struct nhmm_alphabet
{
    rapidstring symbols;
    int symbol_idx[NHMM_SYMBOL_ID_MAX + 1];
};

int check_symbols_length(const char *symbols);
int check_symbol_id_range(char symbol_id);

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

int alphabet_has_symbol(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    check_symbol_id_range(symbol_id);
    return alphabet->symbol_idx[(size_t)symbol_id] != -1;
}

int alphabet_symbol_idx(const struct nhmm_alphabet *alphabet,
                                      char symbol_id)
{
    if (check_symbol_id_range(symbol_id))
        return -1;
    return alphabet->symbol_idx[(size_t)symbol_id];
}

char alphabet_symbol_id(const struct nhmm_alphabet *alphabet,
                                      int symbol_idx)
{
    return rs_data_c(&alphabet->symbols)[symbol_idx];
}

int check_symbols_length(const char *symbols)
{
    if (strlen(symbols) > INT_MAX) {
        error("symbols length is greater than %d", INT_MAX);
        return 1;
    }
    return 0;
}

int check_symbol_id_range(char symbol_id)
{
    if (symbol_id < NHMM_SYMBOL_ID_MIN || symbol_id > NHMM_SYMBOL_ID_MAX) {
        error("symbols must be non-extended ASCII characters");
        return 1;
    }
    return 0;
}

