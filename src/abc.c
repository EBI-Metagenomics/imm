#include "imm/abc.h"
#include "cast.h"
#include "free.h"
#include "imm/abc_types.h"
#include "imm/report.h"
#include <string.h>

struct abc_chunk
{
    uint8_t nsymbols;
    char*   symbols;
    char    any_symbol;
};

static struct imm_abc_vtable const __vtable = {__imm_abc_type_id, __imm_abc_write,
                                               __imm_abc_destroy, __imm_abc_clone};

struct imm_abc const* imm_abc_clone(struct imm_abc const* abc) { return abc->vtable.clone(abc); }

struct imm_abc const* imm_abc_create(char const* symbols, char const any_symbol)
{
    return __imm_abc_create(symbols, any_symbol, NULL);
}

void imm_abc_destroy(struct imm_abc const* abc) { abc->vtable.destroy(abc); }

struct imm_abc* imm_abc_read(FILE* stream)
{
    struct abc_chunk chunk = {.nsymbols = 0, .symbols = NULL, .any_symbol = '\0'};

    if (fread(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1) {
        imm_error("could not read nsymbols");
        goto err;
    }

    chunk.symbols = malloc(sizeof(*chunk.symbols) * (size_t)(chunk.nsymbols + 1));

    if (fread(chunk.symbols, sizeof(*chunk.symbols), (size_t)(chunk.nsymbols + 1), stream) <
        (size_t)(chunk.nsymbols + 1)) {
        imm_error("could not read symbols");
        goto err;
    }

    if (chunk.symbols[chunk.nsymbols] != '\0') {
        imm_error("missing null character");
        goto err;
    }

    if (fread(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1) {
        imm_error("could not read any_symbol");
        goto err;
    }

    struct imm_abc* abc = __imm_abc_create(chunk.symbols, chunk.any_symbol, NULL);
    free_c(chunk.symbols);

    return abc;

err:
    if (chunk.symbols)
        free_c(chunk.symbols);

    return NULL;
}

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const* abc, char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}

int imm_abc_write(struct imm_abc const* abc, FILE* stream)
{
    return abc->vtable.write(abc, stream);
}

struct imm_abc const* __imm_abc_clone(struct imm_abc const* abc)
{
    struct imm_abc* nabc = malloc(sizeof(struct imm_abc));

    nabc->symbols = strdup(abc->symbols);
    nabc->length = abc->length;
    memcpy(nabc->symbol_idx, abc->symbol_idx, sizeof(*abc->symbol_idx) * IMM_SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;
    nabc->vtable = abc->vtable;
    nabc->derived = NULL;

    return nabc;
}

struct imm_abc* __imm_abc_create(char const* symbols, char any_symbol, void* derived)
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

    abc->length = cast_zu_u8(strlen(symbols));
    for (uint8_t i = 0; i < abc->length; ++i) {
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

    abc->vtable = __vtable;
    abc->derived = derived;

    return abc;
}

void __imm_abc_destroy(struct imm_abc const* abc)
{
    free_c(abc->symbols);
    free_c(abc);
}

uint8_t __imm_abc_type_id(struct imm_abc const* abc) { return IMM_ABC_TYPE_ID; }

int __imm_abc_write(struct imm_abc const* abc, FILE* stream)
{
    struct abc_chunk chunk = {.nsymbols = cast_zu_u8(strlen(abc->symbols)),
                              .symbols = (char*)abc->symbols,
                              .any_symbol = abc->any_symbol};

    if (fwrite(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1) {
        imm_error("could not write nsymbols");
        return 1;
    }

    if (fwrite(chunk.symbols, sizeof(*chunk.symbols), (size_t)(chunk.nsymbols + 1), stream) <
        (size_t)(chunk.nsymbols + 1)) {
        imm_error("could not write symbols");
        return 1;
    }

    if (fwrite(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1) {
        imm_error("could not write any_symbol");
        return 1;
    }

    return 0;
}
