#include "cwpack_stream_context.h"
#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_rna const *rna_out = &imm_rna_iupac;

    new_pack_ctx(TMPDIR "/rna.imm");
    EQ(imm_abc_pack((struct imm_abc const *)rna_out, pack_ctx), IMM_SUCCESS);
    del_pack_ctx();

    struct imm_rna rna_in;

    new_unpack_ctx(TMPDIR "/rna.imm");
    EQ(imm_abc_unpack((struct imm_abc *)&rna_in, unpack_ctx), IMM_SUCCESS);
    del_unpack_ctx();

    struct imm_abc const *out = imm_super(imm_super(rna_out));
    struct imm_abc const *in = imm_super(imm_super(&rna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
