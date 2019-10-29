#include "imm/imm.h"

int main(void)
{
    struct imm_abc* abc = imm_abc_create("ACGT");
    imm_abc_destroy(abc);

    return 0;
}
