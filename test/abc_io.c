#include "hope/hope.h"
#include "imm/imm.h"

int main(void)
{
    struct imm_abc abc_out = imm_abc_empty;

    imm_abc_init(&abc_out, IMM_STR("ACGT"), '*');
    FILE *file = fopen(TMPDIR "/abc.imm", "wb");
    EQ(imm_abc_write(&abc_out, file), IMM_SUCCESS);
    fclose(file);

    struct imm_abc abc_in = imm_abc_empty;
    file = fopen(TMPDIR "/abc.imm", "rb");
    EQ(imm_abc_read(&abc_in, file), IMM_SUCCESS);
    fclose(file);

    EQ(abc_in.any_symbol_id, abc_out.any_symbol_id);
    EQ(abc_in.size, abc_out.size);
    EQ(abc_in.vtable.typeid, abc_out.vtable.typeid);
    /* EQ(abc_in.symbols, abc_out.symbols); */

    return hope_status();
}
