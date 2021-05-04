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
    unsigned state_ids[] = {42, 19};
    uint8_t seqlens[] = {3, 5};

    struct imm_path path;
    imm_path_init(&path);

    imm_path_add(&path, IMM_STEP(state_ids[0], seqlens[0]));
    imm_path_add(&path, IMM_STEP(state_ids[1], seqlens[1]));

    EQ(imm_path_step(&path, 0)->state_id, state_ids[0]);
    EQ(imm_path_step(&path, 1)->state_id, state_ids[1]);

    EQ(imm_path_step(&path, 0)->seqlen, seqlens[0]);
    EQ(imm_path_step(&path, 1)->seqlen, seqlens[1]);

    imm_deinit(&path);
}

void test_path_long(void)
{
    struct imm_path path;
    imm_path_init(&path);

    for (uint16_t i = 0; i < 1 << 14; ++i)
        imm_path_add(&path, IMM_STEP(i, i % 16));

    for (uint16_t i = 0; i < 1 << 14; ++i)
    {
        EQ(imm_path_step(&path, i)->state_id, i);
        EQ(imm_path_step(&path, i)->seqlen, i % 16);
    }

    imm_path_reset(&path);

    for (uint16_t i = 0; i < 1 << 14; ++i)
        imm_path_add(&path, IMM_STEP(i, i % 16));

    for (uint16_t i = 0; i < 1 << 14; ++i)
    {
        EQ(imm_path_step(&path, i)->state_id, i);
        EQ(imm_path_step(&path, i)->seqlen, i % 16);
    }

    imm_deinit(&path);
}
