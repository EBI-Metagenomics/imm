#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_amino const *amino_out = &imm_amino_iupac;

    FILE *file = fopen(TMPDIR "/amino.imm", "wb");
    imm_amino_write(amino_out, file);
    fclose(file);

    struct imm_amino amino_in;
    file = fopen(TMPDIR "/amino.imm", "rb");
    imm_amino_read(&amino_in, file);
    fclose(file);

    struct imm_abc const *out = imm_super(amino_out);
    struct imm_abc const *in = imm_super(&amino_in);

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
