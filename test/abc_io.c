#include "hope.h"
#include "imm/imm.h"
#include "lip/file/file.h"

int main(void)
{
    struct lip_file file = {0};
    struct imm_abc abc_out = imm_abc_empty;

    imm_abc_init(&abc_out, IMM_STR("ACGT"), '*');

    file.fp = fopen(TMPDIR "/abc.imm", "wb");
    eq(imm_abc_pack(&abc_out, &file), IMM_OK);
    fclose(file.fp);

    struct imm_abc abc_in = imm_abc_empty;

    file.fp = fopen(TMPDIR "/abc.imm", "rb");
    eq(imm_abc_unpack(&abc_in, &file), IMM_OK);
    fclose(file.fp);

    eq(abc_in.any_symbol_id, abc_out.any_symbol_id);
    eq(abc_in.size, abc_out.size);
    eq(abc_in.vtable.typeid, abc_out.vtable.typeid);
    eq(abc_in.symbols, abc_out.symbols);

    return hope_status();
}
