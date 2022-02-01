#include "abc.h"
#include "cwpack.h"
#include "cwpack_utils.h"
#include "error.h"
#include "imm/sym.h"
#include "xcmp.h"
#include <assert.h>
#include <stdint.h>

struct imm_abc const imm_abc_empty = ABC_EMPTY;

enum imm_rc imm_abc_init(struct imm_abc *abc, struct imm_str symbols,
                         char any_symbol)
{
    struct imm_abc_vtable vtable = {IMM_ABC, NULL};
    return abc_init(abc, symbols.len, symbols.data, any_symbol, vtable);
}

enum imm_rc imm_abc_pack(struct imm_abc const *abc, struct cw_pack_context *ctx)
{
    enum imm_rc rc = abc_pack(abc, ctx);
    if (rc) return error(rc, "failed to write alphabet");
    return rc;
}

enum imm_rc imm_abc_unpack(struct imm_abc *abc, struct cw_unpack_context *ctx)
{
    enum imm_rc rc = abc_unpack(abc, ctx);
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

enum imm_rc abc_pack(struct imm_abc const *abc, struct cw_pack_context *ctx)
{
    cw_pack_map_size(ctx, 4);

    cw_pack_cstr(ctx, "symbols");
    cw_pack_str(ctx, abc->symbols, abc->size);

    cw_pack_cstr(ctx, "idx");
    cw_pack_array_size(ctx, IMM_ARRAY_SIZE(abc->sym.idx));

    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        cw_pack_unsigned(ctx, abc->sym.idx[i]);

    cw_pack_cstr(ctx, "any_symbol_id");
    cw_pack_unsigned(ctx, abc->any_symbol_id);
    cw_pack_cstr(ctx, "typeid");
    cw_pack_unsigned(ctx, abc->vtable.typeid);

    return ctx->return_code == CWP_RC_OK ? IMM_SUCCESS : IMM_FAILURE;
}

enum imm_rc abc_unpack(struct imm_abc *abc, struct cw_unpack_context *ctx)
{
    if (cw_unpack_next_map_size(ctx) != 4) return IMM_FAILURE;

    if (!cw_unpack_next_cstr_expect(ctx, "symbols")) return IMM_FAILURE;

    if (cw_unpack_next_str_lengh(ctx) + 1 > IMM_ARRAY_SIZE(abc->symbols))
        return IMM_FAILURE;
    abc->size = ctx->item.as.str.length;
    cw_unpack_cstr(ctx, abc->symbols);

    if (!cw_unpack_next_cstr_expect(ctx, "idx")) return IMM_FAILURE;

    if (cw_unpack_next_array_size(ctx) != IMM_ARRAY_SIZE(abc->sym.idx))
        return IMM_PARSEERROR;

    for (unsigned i = 0; i < IMM_ARRAY_SIZE(abc->sym.idx); ++i)
        abc->sym.idx[i] = cw_unpack_next_unsigned8(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, "any_symbol_id")) return IMM_FAILURE;
    abc->any_symbol_id = cw_unpack_next_unsigned8(ctx);

    if (!cw_unpack_next_cstr_expect(ctx, "typeid")) return IMM_FAILURE;
    abc->vtable.typeid = cw_unpack_next_unsigned8(ctx);
    if (!imm_abc_typeid_valid(abc->vtable.typeid)) return IMM_PARSEERROR;

    return IMM_SUCCESS;
}
