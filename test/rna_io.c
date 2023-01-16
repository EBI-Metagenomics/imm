#include "hope.h"
#include "imm/imm.h"
#include "lip/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_rna const *rna_out = &imm_rna_iupac;

    file.fp = fopen(TMPDIR "/rna.imm", "wb");
    eq(imm_abc_pack((struct imm_abc const *)rna_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_rna rna_in;

    file.fp = fopen(TMPDIR "/rna.imm", "rb");
    eq(imm_abc_unpack((struct imm_abc *)&rna_in, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc const *out = imm_super(imm_super(rna_out));
    struct imm_abc const *in = imm_super(imm_super(&rna_in));

    eq(in->any_symbol_id, out->any_symbol_id);
    eq(in->size, out->size);
    eq(in->vtable.typeid, out->vtable.typeid);
    eq(in->symbols, out->symbols);

    return hope_status();
}
