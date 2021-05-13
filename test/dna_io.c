#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_dna const *dna_out = &imm_dna_default;

    FILE *file = fopen(TMPDIR "/dna.imm", "wb");
    imm_dna_write(dna_out, file);
    fclose(file);

    struct imm_dna dna_in;
    file = fopen(TMPDIR "/dna.imm", "rb");
    imm_dna_read(&dna_in, file);
    fclose(file);

    struct imm_abc const *out = imm_super(imm_super(dna_out));
    struct imm_abc const *in = imm_super(imm_super(&dna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
