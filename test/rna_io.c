#include "hope.h"
#include "imm/imm.h"
#include "lite_pack/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_rna const *rna_out = &imm_rna_iupac;

    file.fp = fopen(TMPDIR "/rna.imm", "wb");
    EQ(imm_abc_pack((struct imm_abc const *)rna_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_rna rna_in;

    file.fp = fopen(TMPDIR "/rna.imm", "rb");
    EQ(imm_abc_unpack((struct imm_abc *)&rna_in, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc const *out = imm_super(imm_super(rna_out));
    struct imm_abc const *in = imm_super(imm_super(&rna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
