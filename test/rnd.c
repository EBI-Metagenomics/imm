#include "hope.h"
#include "imm/imm.h"

void test_rnd(void);

int main(void)
{
    test_rnd();
    return hope_status();
}

void test_rnd(void)
{
    struct imm_rnd rnd = imm_rnd(1);
    CLOSE(imm_rnd_dbl(&rnd), 0.01092079223);
    CLOSE(imm_rnd_dbl(&rnd), 0.8859520411);
    EQ(imm_rnd_u64(&rnd), 2922809869868169223ULL);
    imm_float arr[2];
    imm_lprob_sample(&rnd, 2, arr);
    CLOSE(arr[0], -0.32597934748);
    CLOSE(arr[1], -1.05687606547);
}
