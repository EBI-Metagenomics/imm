#include "imm/abc.h"
#include "abc.h"
#include "cast.h"
#include "free.h"
#include "imm/report.h"
#include <string.h>

struct abc_chunk
{
    uint8_t nsymbols;
    char*   symbols;
    char    any_symbol;
};

struct imm_abc const* imm_abc_create(char const* symbols, char const any_symbol)
{
    if (any_symbol < IMM_FIRST_CHAR || any_symbol > IMM_LAST_CHAR) {
        imm_error("any_symbol is outside the range [%c, %c] ", IMM_FIRST_CHAR, IMM_LAST_CHAR);
        return NULL;
    }
    struct imm_abc* abc = malloc(sizeof(struct imm_abc));
    abc->any_symbol = any_symbol;

    for (size_t i = 0; i < IMM_SYMBOL_IDX_SIZE; ++i)
        abc->symbol_idx[i] = IMM_ABC_INVALID_IDX;

    if (strlen(symbols) > IMM_SYMBOL_IDX_SIZE) {
        imm_error("alphabet size cannot be larger than %zu", IMM_SYMBOL_IDX_SIZE);
        free_c(abc);
        return NULL;
    }

    abc->length = cast_u_zu(strlen(symbols));
    for (unsigned i = 0; i < abc->length; ++i) {
        if (symbols[i] == any_symbol) {
            imm_error("any_symbol cannot be in the alphabet");
            free_c(abc);
            return NULL;
        }

        if (symbols[i] < IMM_FIRST_CHAR || symbols[i] > IMM_LAST_CHAR) {
            imm_error("alphabet symbol is outside the range [%c, %c] ", IMM_FIRST_CHAR,
                      IMM_LAST_CHAR);
            free_c(abc);
            return NULL;
        }

        size_t j = __imm_abc_index(symbols[i]);
        if (abc->symbol_idx[j] != IMM_ABC_INVALID_IDX) {
            imm_error("alphabet cannot have duplicated symbols");
            free_c(abc);
            return NULL;
        }
        abc->symbol_idx[j] = i;
    }
    abc->symbols = strdup(symbols);

    return abc;
}

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    nabc->length = abc->length;
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(*abc->symbol_idx) * IMM_SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;

    return nabc;
}

void imm_abc_destroy(struct imm_abc const* abc)
{
    free_c(abc->symbols);
    free_c(abc);
}

int abc_write(FILE* stream, struct imm_abc const* abc)
{
    struct abc_chunk chunk = {.nsymbols = cast_u8_zu(strlen(abc->symbols)),
                              .symbols = (char*)abc->symbols,
                              .any_symbol = abc->any_symbol};

    if (fwrite(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1)
        return 1;

    if (fwrite(chunk.symbols, sizeof(*chunk.symbols), chunk.nsymbols + 1, stream) <
        chunk.nsymbols + 1)
        return 1;

    if (fwrite(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1)
        return 1;

    return 0;
}

struct imm_abc const* abc_read(FILE* stream)
{
    struct abc_chunk chunk = {.nsymbols = 0, .symbols = NULL, .any_symbol = '\0'};

    if (fread(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1)
        return NULL;

    chunk.symbols = malloc(sizeof(*chunk.symbols) * (chunk.nsymbols + 1));

    if (fread(chunk.symbols, sizeof(*chunk.symbols), chunk.nsymbols + 1, stream) <
        chunk.nsymbols + 1) {
        free_c(chunk.symbols);
        return NULL;
    }

    if (fread(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1) {
        free_c(chunk.symbols);
        return NULL;
    }

    struct imm_abc* abc = malloc(sizeof(*abc));
    abc->symbols = chunk.symbols;
    abc->length = chunk.nsymbols;

    for (uint8_t i = 0; i < IMM_SYMBOL_IDX_SIZE; ++i)
        abc->symbol_idx[i] = IMM_ABC_INVALID_IDX;

    for (uint8_t i = 0; i < chunk.nsymbols; ++i)
        abc->symbol_idx[__imm_abc_index(chunk.symbols[i])] = i;

    abc->any_symbol = chunk.any_symbol;

    return abc;
}

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}
