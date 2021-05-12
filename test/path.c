#include "hope/hope.h"
#include "imm/imm.h"

void test_path(void);
void test_path_long(void);

int main(void)
{
    test_path();
    test_path_long();
    return hope_status();
}

void test_path(void)
{
    imm_state_id_t state_ids[] = {42, 19};
    imm_state_seqlen_t seqlens[] = {3, 5};

    struct imm_path path = imm_path();

    imm_path_add(&path, imm_step(state_ids[0], seqlens[0]));
    imm_path_add(&path, imm_step(state_ids[1], seqlens[1]));

    EQ(imm_path_step(&path, 0)->state_id, state_ids[0]);
    EQ(imm_path_step(&path, 1)->state_id, state_ids[1]);

    EQ(imm_path_step(&path, 0)->seqlen, seqlens[0]);
    EQ(imm_path_step(&path, 1)->seqlen, seqlens[1]);

    imm_deinit(&path);
}

void test_path_long(void)
{
    struct imm_path path = imm_path();

    for (imm_state_id_t i = 0; i < 1 << 14; ++i)
        imm_path_add(&path, imm_step(i, i % 16));

    for (imm_state_id_t i = 0; i < 1 << 14; ++i)
    {
        EQ(imm_path_step(&path, i)->state_id, i);
        EQ(imm_path_step(&path, i)->seqlen, i % 16);
    }

    imm_path_reset(&path);

    for (imm_state_id_t i = 0; i < 1 << 14; ++i)
        imm_path_add(&path, imm_step(i, i % 16));

    for (imm_state_id_t i = 0; i < 1 << 14; ++i)
    {
        EQ(imm_path_step(&path, i)->state_id, i);
        EQ(imm_path_step(&path, i)->seqlen, i % 16);
    }

    imm_deinit(&path);
}
