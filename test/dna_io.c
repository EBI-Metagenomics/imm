#include "hope.h"
#include "imm/imm.h"
#include "lite_pack/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_dna const *dna_out = &imm_dna_iupac;

    file.fp = fopen(TMPDIR "/dna.imm", "wb");
    EQ(imm_abc_pack((struct imm_abc const *)dna_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_dna dna_in;

    file.fp = fopen(TMPDIR "/dna.imm", "rb");
    EQ(imm_abc_unpack((struct imm_abc *)&dna_in, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc const *out = imm_super(imm_super(dna_out));
    struct imm_abc const *in = imm_super(imm_super(&dna_in));

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
