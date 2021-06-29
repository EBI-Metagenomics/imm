#include "imm/imm.h"

void dp_normal(void);
void dp_frame(void);

int main(void)
{
    dp_normal();
    dp_frame();
    return 0;
}

void dp_normal(void)
{
    imm_example1_init();
    struct imm_example1 *m = &imm_example1;
    struct imm_dp *dp = imm_hmm_new_dp(imm_example1.hmm, imm_super(m->end));

    FILE *file = fopen(CORPUS "/dp_normal.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);
    imm_example1_deinit();
}

void dp_frame(void)
{
    imm_example2_init();
    struct imm_example2 *m = &imm_example2;
    struct imm_dp *dp = imm_hmm_new_dp(m->hmm, imm_super(m->end));

    FILE *file = fopen(CORPUS "/dp_frame.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);
    imm_example2_deinit();
}
