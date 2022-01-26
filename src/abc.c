#include "abc.h"
#include "cmp/cmp.h"
#include "error.h"
#include "imm/sym.h"
#include <assert.h>
#include <stdint.h>

struct imm_abc const imm_abc_empty = ABC_EMPTY;

enum imm_rc imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.len, symbols.data, any_symbol, vtable);
}

enum imm_rc imm_abc_write(struct imm_abc const *abc, FILE *file)
{
    enum imm_rc rc = abc_write(abc, file);
    if (rc) return error(rc, "failed to write alphabet");
    return rc;
}

enum imm_rc imm_abc_read(struct imm_abc *abc, FILE *file)
{
    enum imm_rc rc = abc_read(abc, file);
    if (rc) return error(rc, "failed to read alphabet");
    return rc;
}

unsigned imm_abc_union_size(struct imm_abc const *abc, struct imm_str seq)
{
    unsigned size = 0;
    for (unsigned i = 0; i < seq.len; ++i)
    {
        char const any = imm_abc_any_symbol(abc);
        size += !imm_abc_has_symbol(abc, seq.data[i]) && seq.data[i] != any;
    }
    return size;
}

enum imm_rc abc_init(struct imm_abc *abc, unsigned len, char const *symbols,
                     char any_symbol, struct imm_abc_vtable vtable)
{
    if (!imm_sym_valid_char(any_symbol))
        return error(IMM_ILLEGALARG, "any_symbol outside range");

    if (len == 0) return error(IMM_ILLEGALARG, "alphabet cannot be empty");

    if (len > IMM_ABC_MAX_SIZE || len > IMM_SYM_SIZE)
        return error(IMM_ILLEGALARG, "symbols length is too large");

    abc->size = len;
    memcpy(abc->symbols, symbols, sizeof *abc->symbols * len);
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

#define ERETURN(expr, e)                                                       \
    do                                                                         \
    {                                                                          \
        if (!!(expr)) return e;                                                \
    } while (0)

enum imm_rc abc_write(struct imm_abc const *abc, FILE *file)
{
    cmp_ctx_t cmp = {0};
    cmp_setup(&cmp, file);

    cmp_write_map(&cmp, 4);

    CMP_WRITE_STR(&cmp, "symbols");
    ERETURN(!cmp_write_str(&cmp, abc->symbols, abc->size), IMM_IOERROR);

    CMP_WRITE_STR(&cmp, "idx");
    ERETURN(!cmp_write_array(&cmp, IMM_ARRAY_SIZE(abc->sym.idx)), IMM_IOERROR);

    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        ERETURN(!cmp_write_u8(&cmp, abc->sym.idx[i]), IMM_IOERROR);

    CMP_WRITE_STR(&cmp, "any_symbol_id");
    ERETURN(!cmp_write_u8(&cmp, (uint8_t)abc->any_symbol_id), IMM_IOERROR);
    CMP_WRITE_STR(&cmp, "typeid");
    ERETURN(!cmp_write_u8(&cmp, (uint8_t)abc->vtable.typeid), IMM_IOERROR);

    return IMM_SUCCESS;
}

enum imm_rc abc_read(struct imm_abc *abc, FILE *file)
{
    cmp_ctx_t cmp = {0};
    cmp_setup(&cmp, file);

    uint32_t u32 = 0;
    cmp_read_map(&cmp, &u32);
    assert(u32 == 4);

    ERETURN(!cmp_skip_str(&cmp), IMM_IOERROR);
    u32 = IMM_ARRAY_SIZE(abc->symbols) - 1;
    ERETURN(!cmp_read_cstr(&cmp, abc->symbols, &u32), IMM_IOERROR);
    abc->size = u32;
    abc->symbols[abc->size] = '\0';

    ERETURN(!cmp_skip_str(&cmp), IMM_IOERROR);
    ERETURN(!cmp_read_array(&cmp, &u32), IMM_IOERROR);
    ERETURN(IMM_ARRAY_SIZE(abc->sym.idx) != u32, IMM_PARSEERROR);
    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        ERETURN(!cmp_read_u8(&cmp, abc->sym.idx + i), IMM_IOERROR);

    uint8_t u8 = 0;
    ERETURN(!cmp_skip_str(&cmp), IMM_IOERROR);
    ERETURN(!cmp_read_u8(&cmp, &u8), IMM_IOERROR);
    abc->any_symbol_id = u8;

    ERETURN(!cmp_skip_str(&cmp), IMM_IOERROR);
    ERETURN(!cmp_read_u8(&cmp, &u8), IMM_IOERROR);
    ERETURN(!imm_abc_typeid_valid(u8), IMM_PARSEERROR);
    abc->vtable.typeid = u8;
    return IMM_SUCCESS;
}
