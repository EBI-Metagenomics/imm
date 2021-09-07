#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_rna const *rna_out = &imm_rna_iupac;

    FILE *file = fopen(TMPDIR "/rna.imm", "wb");
    imm_rna_write(rna_out, file);
    fclose(file);

    struct imm_rna rna_in;
    file = fopen(TMPDIR "/rna.imm", "rb");
    imm_rna_read(&rna_in, file);
    fclose(file);

    struct imm_abc const *out = imm_super(imm_super(rna_out));
    struct imm_abc const *in = imm_super(imm_super(&rna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
