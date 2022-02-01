#include "cwpack_stream_context.h"
#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_abc abc_out = imm_abc_empty;

    imm_abc_init(&abc_out, IMM_STR("ACGT"), '*');

    new_pack_ctx(TMPDIR "/abc.imm");
    EQ(imm_abc_pack(&abc_out, pack_ctx), IMM_SUCCESS);
    del_pack_ctx();

    struct imm_abc abc_in = imm_abc_empty;

    new_unpack_ctx(TMPDIR "/abc.imm");
    EQ(imm_abc_unpack(&abc_in, unpack_ctx), IMM_SUCCESS);
    del_unpack_ctx();

    EQ(abc_in.any_symbol_id, abc_out.any_symbol_id);
    EQ(abc_in.size, abc_out.size);
    EQ(abc_in.vtable.typeid, abc_out.vtable.typeid);
    EQ(abc_in.symbols, abc_out.symbols);

    return hope_status();
}
