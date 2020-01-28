#include "imm/abc.h"
#include "free.h"
#include "imm/report.h"
#include <string.h>

#define FIRST_VISUAL_CHAR '!'
#define LAST_VISUAL_CHAR '~'

#define SYMBOL_IDX_SIZE ((size_t)((LAST_VISUAL_CHAR - FIRST_VISUAL_CHAR) + 1))

/**
 * Immutable alphabet.
 *
 * It represents a finite set of symbols and a special any-symbol
 * symbol.
 */
struct imm_abc
{
    char const* symbols;
    unsigned    length;
    int         symbol_idx[SYMBOL_IDX_SIZE];
    char        any_symbol;
};

static inline size_t char_to_index(char const c) { return (size_t)(c - FIRST_VISUAL_CHAR); }

struct imm_abc const* imm_abc_create(char const* symbols, char const any_symbol)
{
    if (any_symbol < FIRST_VISUAL_CHAR || any_symbol > LAST_VISUAL_CHAR) {
        imm_error("any_symbol is outside the range [%c, %c] ", FIRST_VISUAL_CHAR,
                  LAST_VISUAL_CHAR);
        return NULL;
    }
    struct imm_abc* abc = malloc(sizeof(struct imm_abc));
    abc->any_symbol = any_symbol;

    for (size_t i = 0; i < SYMBOL_IDX_SIZE; ++i)
        abc->symbol_idx[i] = -1;

    if (strlen(symbols) > SYMBOL_IDX_SIZE) {
        imm_error("alphabet size cannot be larger than %zu", SYMBOL_IDX_SIZE);
        free_c(abc);
        return NULL;
    }

    abc->length = (unsigned)strlen(symbols);
    for (unsigned i = 0; i < abc->length; ++i) {
        if (symbols[i] == any_symbol) {
            imm_error("any_symbol cannot be in the alphabet");
            free_c(abc);
            return NULL;
        }

        if (symbols[i] < FIRST_VISUAL_CHAR || symbols[i] > LAST_VISUAL_CHAR) {
            imm_error("alphabet symbol is outside the range [%c, %c] ", FIRST_VISUAL_CHAR,
                      LAST_VISUAL_CHAR);
            return NULL;
        }

        size_t j = char_to_index(symbols[i]);
        if (abc->symbol_idx[j] != -1) {
            imm_error("alphabet cannot have duplicated symbols");
            free_c(abc);
            return NULL;
        }
        abc->symbol_idx[j] = (int)i;
    }
    abc->symbols = strdup(symbols);

    return abc;
}

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    nabc->length = abc->length;
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(int) * SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;

    return nabc;
}

void imm_abc_destroy(struct imm_abc const* abc)
{
    free_c(abc->symbols);
    free_c(abc);
}

unsigned imm_abc_length(struct imm_abc const* abc) { return abc->length; }

char const* imm_abc_symbols(struct imm_abc const* abc) { return abc->symbols; }

bool imm_abc_has_symbol(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id < FIRST_VISUAL_CHAR || symbol_id > LAST_VISUAL_CHAR)
        return false;

    return abc->symbol_idx[char_to_index(symbol_id)] != -1;
}

int imm_abc_symbol_idx(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id < FIRST_VISUAL_CHAR || symbol_id > LAST_VISUAL_CHAR)
        return -1;
    return abc->symbol_idx[char_to_index(symbol_id)];
}

char imm_abc_symbol_id(struct imm_abc const* abc, unsigned symbol_idx)
{
    return abc->symbols[symbol_idx];
}

char imm_abc_any_symbol(struct imm_abc const* abc) { return abc->any_symbol; }

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}
