#include "hope/hope.h"
#include "imm/imm.h"
#include "lite_pack/io/file.h"

int main(void)
{
    struct lip_io_file io = {0};
    struct imm_amino const *amino_out = &imm_amino_iupac;

    io.fp = fopen(TMPDIR "/amino.imm", "wb");
    EQ(imm_abc_pack((struct imm_abc const *)amino_out, &io), IMM_SUCCESS);
    fclose(io.fp);

    struct imm_amino amino_in;

    io.fp = fopen(TMPDIR "/amino.imm", "rb");
    EQ(imm_abc_unpack((struct imm_abc *)&amino_in, &io), IMM_SUCCESS);
    fclose(io.fp);

    struct imm_abc const *out = imm_super(amino_out);
    struct imm_abc const *in = imm_super(&amino_in);

    EQ(in->any_symbol_id, out->any_symbol_id);
    EQ(in->size, out->size);
    EQ(in->vtable.typeid, out->vtable.typeid);
    EQ(in->symbols, out->symbols);

    return hope_status();
}
