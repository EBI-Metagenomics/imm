#include "imm/imm.h"
#include "model_frame.h"
#include "model_normal.h"

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
    model_normal_init();
    struct model_normal *m = &model_normal;
    struct imm_dp *dp = imm_hmm_new_dp(model_normal.hmm, imm_super(m->end));

    FILE *file = fopen(CORPUS "/dp_normal.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);
    model_normal_deinit();
}

void dp_frame(void)
{
    model_frame_init();
    struct model_frame *m = &model_frame;
    struct imm_dp *dp = imm_hmm_new_dp(m->hmm, imm_super(m->end));

    FILE *file = fopen(CORPUS "/dp_frame.imm", "wb");
    imm_dp_write(dp, file);
    fclose(file);
    imm_del(dp);
    model_frame_deinit();
}
