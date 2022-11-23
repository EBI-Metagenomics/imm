#include "abc.h"
#include "error.h"
#include "expect.h"
#include "imm/sym.h"
#include "lite_pack.h"
#include <assert.h>
#include <stdint.h>
#include <string.h>

struct imm_abc const imm_abc_empty = ABC_EMPTY;

enum imm_rc imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return imm__abc_init(abc, symbols.len, symbols.data, any_symbol, vtable);
}

enum imm_rc imm_abc_pack(struct imm_abc const *abc, struct lip_file *file)
{
    enum imm_rc rc = imm__abc_pack(abc, file);
    return rc ? error(IMM_IOERROR) : rc;
}

enum imm_rc imm_abc_unpack(struct imm_abc *abc, struct lip_file *file)
{
    enum imm_rc rc = imm__abc_unpack(abc, file);
    return rc ? error(IMM_IOERROR) : rc;
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

enum imm_rc imm__abc_init(struct imm_abc *abc, unsigned len,
                          char const *symbols, char any_symbol,
                          struct imm_abc_vtable vtable)
{
    if (!imm_sym_valid_char(any_symbol)) return error(IMM_ANY_SYMBOL_OUT_RANGE);

    if (len == 0) return error(IMM_EMPTY_ABC);

    if (len > IMM_ABC_MAX_SIZE || len > IMM_SYM_SIZE)
        return error(IMM_TOO_MANY_SYMBOLS);

    abc->size = len;
    memcpy(abc->symbols, symbols, sizeof *abc->symbols * len);
    abc->symbols[abc->size] = '\0';
    imm_sym_init(&abc->sym);
    abc->any_symbol_id = imm_sym_id(any_symbol);

    for (unsigned i = 0; i < abc->size; ++i)
    {
        if (symbols[i] == any_symbol) return error(IMM_ANY_SYMBOL_IN_ABC);

        if (!imm_sym_valid_char(symbols[i]))
            return error(IMM_SYMBOL_OUT_OF_RANGE);

        unsigned id = imm_sym_id(symbols[i]);
        if (imm_sym_idx(&abc->sym, id) != IMM_SYM_NULL_IDX)
            return error(IMM_DUPLICATED_SYMBOLS);

        imm_sym_set_idx(&abc->sym, id, i);
    }
    imm_sym_set_idx(&abc->sym, abc->any_symbol_id, abc->size);
    abc->vtable = vtable;
    return IMM_OK;
}

static_assert(sizeof(imm_sym_id_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_sym_idx_t) == sizeof(uint8_t), "wrong types");
static_assert(sizeof(imm_abc_typeid_t) == sizeof(uint8_t), "wrong types");

#define ERET(expr, e)                                                          \
    do                                                                         \
    {                                                                          \
        if (!!(expr)) return e;                                                \
    } while (0)

enum imm_rc imm__abc_pack(struct imm_abc const *abc, struct lip_file *file)
{
    lip_write_map_size(file, 4);

    lip_write_cstr(file, "symbols");
    lip_write_cstr(file, abc->symbols);

    lip_write_cstr(file, "idx");
    lip_write_1darray_u8(file, IMM_ARRAY_SIZE(abc->sym.idx), abc->sym.idx);

    lip_write_cstr(file, "any_symbol_id");
    lip_write_int(file, abc->any_symbol_id);

    lip_write_cstr(file, "typeid");
    lip_write_int(file, abc->vtable.typeid);

    return file->error ? error(IMM_IOERROR) : IMM_OK;
}

enum imm_rc imm__abc_unpack(struct imm_abc *abc, struct lip_file *file)
{
    if (!imm_expect_map_size(file, 4)) return error(IMM_IOERROR);

    if (!imm_expect_map_key(file, "symbols")) return error(IMM_IOERROR);
    lip_read_cstr(file, IMM_ABC_MAX_SIZE, abc->symbols);

    abc->size = (unsigned)strlen(abc->symbols);

    if (!imm_expect_map_key(file, "idx")) return error(IMM_IOERROR);
    imm_expect_1darr_u8_type(file, IMM_ARRAY_SIZE(abc->sym.idx), abc->sym.idx);

    if (!imm_expect_map_key(file, "any_symbol_id")) return error(IMM_IOERROR);
    lip_read_int(file, &abc->any_symbol_id);

    if (!imm_expect_map_key(file, "typeid")) return error(IMM_IOERROR);
    lip_read_int(file, &abc->vtable.typeid);

    if (!imm_abc_typeid_valid(abc->vtable.typeid)) return error(IMM_IOERROR);

    return file->error ? error(IMM_IOERROR) : IMM_OK;
}
