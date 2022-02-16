#include "hope/hope.h"
#include "imm/imm.h"
#include "lite_pack/io/file.h"

int main(void)
{
    struct lip_io_file io = {0};
    struct imm_abc abc_out = imm_abc_empty;

    imm_abc_init(&abc_out, IMM_STR("ACGT"), '*');

    io.fp = fopen(TMPDIR "/abc.imm", "wb");
    EQ(imm_abc_pack(&abc_out, &io), IMM_SUCCESS);
    fclose(io.fp);

    struct imm_abc abc_in = imm_abc_empty;

    io.fp = fopen(TMPDIR "/abc.imm", "rb");
    EQ(imm_abc_unpack(&abc_in, &io), IMM_SUCCESS);
    fclose(io.fp);

    EQ(abc_in.any_symbol_id, abc_out.any_symbol_id);
    EQ(abc_in.size, abc_out.size);
    EQ(abc_in.vtable.typeid, abc_out.vtable.typeid);
    EQ(abc_in.symbols, abc_out.symbols);

    return hope_status();
}
