#include "abc.h"
#include "error.h"
#include "imm/sym.h"
#include "xlip.h"
#include <assert.h>
#include <stdint.h>

struct imm_abc const imm_abc_empty = ABC_EMPTY;

enum imm_rc imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.len, symbols.data, any_symbol, vtable);
}

enum imm_rc imm_abc_pack(struct imm_abc const *abc, struct lip_io_file *io)
{
    enum imm_rc rc = abc_pack(abc, io);
    if (rc) return error(rc, "failed to write alphabet");
    return rc;
}

enum imm_rc imm_abc_unpack(struct imm_abc *abc, struct lip_io_file *io)
{
    enum imm_rc rc = abc_unpack(abc, io);
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

#define ERET(expr, e)                                                          \
    do                                                                         \
    {                                                                          \
        if (!!(expr)) return e;                                                \
    } while (0)

enum imm_rc abc_pack(struct imm_abc const *abc, struct lip_io_file *io)
{
    lip_write_map_size(io, 4);

    xlip_write_key(io, "symbols");
    xlip_write_cstr(io, abc->symbols);

    xlip_write_key(io, "idx");
    lip_write_1darray_u8(io, IMM_ARRAY_SIZE(abc->sym.idx), abc->sym.idx);

    xlip_write_key(io, "any_symbol_id");
    lip_write_int(io, abc->any_symbol_id);

    xlip_write_key(io, "typeid");
    lip_write_int(io, abc->vtable.typeid);

    return io->error ? IMM_FAILURE : IMM_SUCCESS;
}

enum imm_rc abc_unpack(struct imm_abc *abc, struct lip_io_file *io)
{
    if (!xlip_expect_map(io, 4)) return IMM_FAILURE;

    if (!xlip_expect_key(io, "symbols")) return IMM_FAILURE;
    xlip_read_cstr(io, IMM_ABC_MAX_SIZE, abc->symbols);

    abc->size = (unsigned)strlen(abc->symbols);

    if (!xlip_expect_key(io, "idx")) return IMM_FAILURE;
    xlip_expect_1darray_u8_type(io, IMM_ARRAY_SIZE(abc->sym.idx),
                                LIP_1DARRAY_UINT8, abc->sym.idx);

    if (!xlip_expect_key(io, "any_symbol_id")) return IMM_FAILURE;
    lip_read_int(io, &abc->any_symbol_id);

    if (!xlip_expect_key(io, "typeid")) return IMM_FAILURE;
    lip_read_int(io, &abc->vtable.typeid);

    if (!imm_abc_typeid_valid(abc->vtable.typeid)) return IMM_PARSEERROR;

    return io->error ? IMM_FAILURE : IMM_SUCCESS;
}
