#include "hope/hope.h"
#include "imm/imm.h"
#include "lite_pack/io/file.h"

int main(void)
{
    struct lip_io_file io = {0};
    struct imm_rna const *rna_out = &imm_rna_iupac;

    io.fp = fopen(TMPDIR "/rna.imm", "wb");
    EQ(imm_abc_pack((struct imm_abc const *)rna_out, &io), IMM_SUCCESS);
    fclose(io.fp);

    struct imm_rna rna_in;

    io.fp = fopen(TMPDIR "/rna.imm", "rb");
    EQ(imm_abc_unpack((struct imm_abc *)&rna_in, &io), IMM_SUCCESS);
    fclose(io.fp);

    struct imm_abc const *out = imm_super(imm_super(rna_out));
    struct imm_abc const *in = imm_super(imm_super(&rna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
