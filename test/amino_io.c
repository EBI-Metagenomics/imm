#include "hope.h"
#include "imm/imm.h"
#include "lip/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_amino const *amino_out = &imm_amino_iupac;

    file.fp = fopen(TMPDIR "/amino.imm", "wb");
    eq(imm_abc_pack((struct imm_abc const *)amino_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_amino amino_in;

    file.fp = fopen(TMPDIR "/amino.imm", "rb");
    eq(imm_abc_unpack((struct imm_abc *)&amino_in, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc const *out = imm_super(amino_out);
    struct imm_abc const *in = imm_super(&amino_in);

    eq(in->any_symbol_id, out->any_symbol_id);
    eq(in->size, out->size);
    eq(in->vtable.typeid, out->vtable.typeid);
    eq(in->symbols, out->symbols);

    return hope_status();
}
