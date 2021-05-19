#include "abc.h"
#include "cmp.h"
#include "imm/log.h"
#include "imm/support.h"
#include "imm/sym.h"
#include "io.h"
#include "support.h"
#include <assert.h>
#include <stdint.h>

struct imm_abc const imm_abc_empty = {
    .size = 0,
    .symbols = {'\0'},
    .sym = {.idx[0 ...(IMM_SYM_SIZE) - 1] = IMM_SYM_NULL_IDX},
    .any_symbol_id = IMM_SYM_ID('X'),
    .vtable = {IMM_ABC, NULL},
};

int imm_abc_init(struct imm_abc *abc, struct imm_str symbols, char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.size, symbols.data, any_symbol, vtable);
}

int imm_abc_write(struct imm_abc const *abc, FILE *file)
{
    return abc_write(abc, file);
}

int imm_abc_read(struct imm_abc *abc, FILE *file)
{
    return abc_read(abc, file);
}

int abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
             char any_symbol, struct imm_abc_vtable vtable)
{
    if (!imm_sym_valid_char(any_symbol))
        return error(IMM_ILLEGALARG, "any_symbol outside range");

    if (len == 0)
        return error(IMM_ILLEGALARG, "alphabet cannot be empty");

    if (len > IMM_MAX_SYMBOLS_SIZE || len > IMM_SYM_SIZE)
        return error(IMM_ILLEGALARG, "symbols length is too large");

    abc->size = len;
    xmemcpy(abc->symbols, symbols, sizeof(*abc->symbols) * len);
    abc->symbols[abc->size] = '\0';
    imm_sym_init(&abc->sym);
    abc->any_symbol_id = imm_sym_id(any_symbol);

    for (unsigned i = 0; i < abc->size; ++i)
    {
        if (symbols[i] == any_symbol)
            return error(IMM_ILLEGALARG,
                         "any_symbol cannot be in the alphabet");

        if (!imm_sym_valid_char(symbols[i]))
            return error(IMM_ILLEGALARG, "symbol outside range");

        unsigned id = imm_sym_id(symbols[i]);
        if (imm_sym_idx(&abc->sym, id) != IMM_SYM_NULL_IDX)
            return error(IMM_ILLEGALARG,
                         "alphabet cannot have duplicated symbols");

        imm_sym_set_idx(&abc->sym, id, i);
    }
    imm_sym_set_idx(&abc->sym, abc->any_symbol_id, abc->size);
    abc->vtable = vtable;
    return IMM_SUCCESS;
}

static_assert(sizeof(imm_sym_id_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_sym_idx_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_abc_typeid_t) == sizeof(uint8_t), "wrong types");

#define ERETURN(expr)                                                          \
    do                                                                         \
    {                                                                          \
        if (!(expr))                                                           \
            return IMM_IOERROR;                                                \
    } while (0)

int abc_write(struct imm_abc const *abc, FILE *file)
{
    cmp_ctx_t cmp = {0};
    cmp_init(&cmp, file, file_reader, file_skipper, file_writer);

    ERETURN(cmp_write_str(&cmp, abc->symbols, abc->size));
    ERETURN(cmp_write_array(&cmp, IMM_ARRAY_SIZE(abc->sym.idx)));

    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        ERETURN(cmp_write_u8(&cmp, abc->sym.idx[i]));

    ERETURN(cmp_write_u8(&cmp, (uint8_t)abc->any_symbol_id));
    ERETURN(cmp_write_u8(&cmp, (uint8_t)abc->vtable.typeid));

    return IMM_SUCCESS;
}

int abc_read(struct imm_abc *abc, FILE *file)
{
    cmp_ctx_t cmp = {0};
    cmp_init(&cmp, file, file_reader, file_skipper, file_writer);

    uint32_t u32 = IMM_ARRAY_SIZE(abc->symbols) - 1;
    ERETURN(cmp_read_str(&cmp, abc->symbols, &u32));
    abc->size = u32;
    abc->symbols[abc->size] = '\0';

    ERETURN(cmp_read_array(&cmp, &u32));
    IMM_BUG(IMM_ARRAY_SIZE(abc->sym.idx) != u32);
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        ERETURN(cmp_read_u8(&cmp, abc->sym.idx + i));

    uint8_t u8 = 0;
    ERETURN(cmp_read_u8(&cmp, &u8));
    abc->any_symbol_id = u8;

    cmp_read_u8(&cmp, &u8);
    abc->vtable.typeid = u8;
    return IMM_SUCCESS;
}
