#include "hope.h"
#include "imm/imm.h"
#include "lip/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_dna const *dna_out = &imm_dna_iupac;

    file.fp = fopen(TMPDIR "/dna.imm", "wb");
    eq(imm_abc_pack((struct imm_abc const *)dna_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_dna dna_in;

    file.fp = fopen(TMPDIR "/dna.imm", "rb");
    eq(imm_abc_unpack((struct imm_abc *)&dna_in, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc const *out = imm_super(imm_super(dna_out));
    struct imm_abc const *in = imm_super(imm_super(&dna_in));

    eq(in->any_symbol_id, out->any_symbol_id);
    eq(in->size, out->size);
    eq(in->vtable.typeid, out->vtable.typeid);
    eq(in->symbols, out->symbols);

    return hope_status();
}
