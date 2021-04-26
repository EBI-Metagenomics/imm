#include "imm/abc.h"
#include "common/common.h"
#include "imm/error.h"

struct abc_chunk
{
    uint8_t nsymbols;
    char *symbols;
    char any_symbol;
};

static struct imm_abc_vtable const __vtable = {
    __imm_abc_type_id, __imm_abc_write, __imm_abc_destroy, __imm_abc_clone};

struct imm_abc const *imm_abc_clone(struct imm_abc const *abc)
{
    return abc->vtable.clone(abc);
}

void imm_abc_del(struct imm_abc const *abc) { abc->vtable.del(abc); }

struct imm_abc *imm_abc_read(FILE *stream)
{
    struct abc_chunk chunk = {
        .nsymbols = 0, .symbols = NULL, .any_symbol = '\0'};

    if (fread(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1)
    {
        xerror(IMM_IOERROR, "could not read nsymbols");
        goto cleanup;
    }

    chunk.symbols =
        malloc(sizeof(*chunk.symbols) * (size_t)(chunk.nsymbols + 1));
    if (!chunk.symbols)
    {
        xerror(IMM_OUTOFMEM, "abc symbols");
        goto cleanup;
    }

    if (fread(chunk.symbols, sizeof(*chunk.symbols),
              (size_t)(chunk.nsymbols + 1),
              stream) < (size_t)(chunk.nsymbols + 1))
    {
        xerror(IMM_IOERROR, "could not read symbols");
        goto cleanup;
    }

    if (chunk.symbols[chunk.nsymbols] != '\0')
    {
        xerror(IMM_IOERROR, "missing null character");
        goto cleanup;
    }

    if (fread(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1)
    {
        xerror(IMM_IOERROR, "could not read any_symbol");
        goto cleanup;
    }

    struct imm_abc *abc =
        __imm_abc_new(chunk.nsymbols, chunk.symbols, chunk.any_symbol, NULL);
    free(chunk.symbols);
    return abc;

cleanup:
    free_if(chunk.symbols);
    return NULL;
}

enum imm_symbol_type imm_abc_symbol_type(struct imm_abc const *abc,
                                         char symbol_id)
{
    if (symbol_id == abc->any_symbol)
        return IMM_SYMBOL_ANY;

    if (imm_abc_has_symbol(abc, symbol_id))
        return IMM_SYMBOL_NORMAL;

    return IMM_SYMBOL_UNKNOWN;
}

int imm_abc_write(struct imm_abc const *abc, FILE *stream)
{
    return abc->vtable.write(abc, stream);
}

struct imm_abc const *__imm_abc_clone(struct imm_abc const *abc)
{
    struct imm_abc *nabc = xmalloc(sizeof(struct imm_abc));
    nabc->symbols = xstrdup(abc->symbols);
    nabc->len = abc->len;
    xmemcpy(nabc->symbol_idx, abc->symbol_idx,
            sizeof(*abc->symbol_idx) * IMM_SYMBOL_IDX_SIZE);
    nabc->any_symbol = abc->any_symbol;
    nabc->vtable = abc->vtable;
    nabc->derived = NULL;
    return nabc;
}

struct imm_abc *__imm_abc_new(uint8_t len, char const *symbols, char any_symbol,
                              void *derived)
{
    if (any_symbol < IMM_FIRST_CHAR || any_symbol > IMM_LAST_CHAR)
    {
        xerror(IMM_ILLEGALARG,
               "any_symbol outside range [" XSTR(IMM_FIRST_CHAR) ", " XSTR(
                   IMM_LAST_CHAR) "] ");
        return NULL;
    }
    if (len == 0)
    {
        xerror(IMM_ILLEGALARG, "alphabet cannot be empty");
        return NULL;
    }

    struct imm_abc *abc = xmalloc(sizeof(*abc));
    abc->any_symbol = any_symbol;

    for (size_t i = 0; i < IMM_SYMBOL_IDX_SIZE; ++i)
        abc->symbol_idx[i] = IMM_ABC_INVALID_IDX;

    if (len > IMM_SYMBOL_IDX_SIZE)
    {
        xerror(IMM_ILLEGALARG, "symbols length is too large");
        free(abc);
        return NULL;
    }

    abc->len = len;
    for (uint8_t i = 0; i < abc->len; ++i)
    {
        if (symbols[i] == any_symbol)
        {
            xerror(IMM_ILLEGALARG, "any_symbol cannot be in the alphabet");
            free(abc);
            return NULL;
        }

        if (symbols[i] < IMM_FIRST_CHAR || symbols[i] > IMM_LAST_CHAR)
        {

            xerror(IMM_ILLEGALARG,
                   "symbol outside range [" XSTR(IMM_FIRST_CHAR) ", " XSTR(
                       IMM_LAST_CHAR) "] ");
            free(abc);
            return NULL;
        }

        size_t j = __imm_abc_index(symbols[i]);
        if (abc->symbol_idx[j] != IMM_ABC_INVALID_IDX)
        {
            xerror(IMM_IOERROR, "alphabet cannot have duplicated symbols");
            free(abc);
            return NULL;
        }
        abc->symbol_idx[j] = i;
    }
    abc->symbols = xstrdup(symbols);

    abc->vtable = __vtable;
    abc->derived = derived;

    return abc;
}

void __imm_abc_destroy(struct imm_abc const *abc)
{
    free((void *)abc->symbols);
    free((void *)abc);
}

uint8_t __imm_abc_type_id(struct imm_abc const *abc) { return IMM_ABC_TYPE_ID; }

int __imm_abc_write(struct imm_abc const *abc, FILE *stream)
{
    struct abc_chunk chunk = {.nsymbols = (uint8_t)strlen(abc->symbols),
                              .symbols = (char *)abc->symbols,
                              .any_symbol = abc->any_symbol};

    if (fwrite(&chunk.nsymbols, sizeof(chunk.nsymbols), 1, stream) < 1)
        return xerror(IMM_IOERROR, "failed to write nsymbols");

    if (fwrite(chunk.symbols, sizeof(*chunk.symbols),
               (size_t)(chunk.nsymbols + 1),
               stream) < (size_t)(chunk.nsymbols + 1))
        return xerror(IMM_IOERROR, "failed to write symbols");

    if (fwrite(&chunk.any_symbol, sizeof(chunk.any_symbol), 1, stream) < 1)
        return xerror(IMM_IOERROR, "failed to write any_symbol");

    return IMM_SUCCESS;
}
