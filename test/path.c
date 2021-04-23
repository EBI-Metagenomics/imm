#include "cass/cass.h"
#include "imm/imm.h"

void test_path(void);
void test_path_long(void);

int main(void)
{
    test_path();
    test_path_long();
    return cass_status();
}

void test_path(void)
{
    uint16_t state_ids[] = {42, 19};
    uint8_t seq_lens[] = {3, 5};

    struct imm_path *path = imm_path_new();

    imm_path_add(path, IMM_STEP(state_ids[0], seq_lens[0]));
    imm_path_add(path, IMM_STEP(state_ids[1], seq_lens[1]));

    cass_equal(imm_path_step(path, 0)->state_id, state_ids[0]);
    cass_equal(imm_path_step(path, 1)->state_id, state_ids[1]);

    cass_equal(imm_path_step(path, 0)->seq_len, seq_lens[0]);
    cass_equal(imm_path_step(path, 1)->seq_len, seq_lens[1]);

    imm_path_del(path);
}

void test_path_long(void)
{
    struct imm_path *path = imm_path_new();

    for (uint16_t i = 0; i < 1 << 14; ++i)
        imm_path_add(path, IMM_STEP(i, i % 16));

    for (uint16_t i = 0; i < 1 << 14; ++i)
    {
        cass_equal(imm_path_step(path, i)->state_id, i);
        cass_equal(imm_path_step(path, i)->seq_len, i % 16);
    }

    imm_path_reset(path);

    for (uint16_t i = 0; i < 1 << 14; ++i)
        imm_path_add(path, IMM_STEP(i, i % 16));

    for (uint16_t i = 0; i < 1 << 14; ++i)
    {
        cass_equal(imm_path_step(path, i)->state_id, i);
        cass_equal(imm_path_step(path, i)->seq_len, i % 16);
    }

    imm_path_del(path);
}
