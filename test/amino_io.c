#include "cwpack_stream_context.h"
#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_amino const *amino_out = &imm_amino_iupac;

    new_pack_ctx(TMPDIR "/amino.imm");
    EQ(imm_abc_pack((struct imm_abc const *)amino_out, pack_ctx), IMM_SUCCESS);
    del_pack_ctx();

    struct imm_amino amino_in;

    new_unpack_ctx(TMPDIR "/amino.imm");
    EQ(imm_abc_unpack((struct imm_abc *)&amino_in, unpack_ctx), IMM_SUCCESS);
    del_unpack_ctx();

    struct imm_abc const *out = imm_super(amino_out);
    struct imm_abc const *in = imm_super(&amino_in);

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
