#include "imm.h"
#include "unity.h"

void test_path(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_path);
    return UNITY_END();
}

void test_path(void)
{
    struct imm_path *path;

    imm_path_create(&path);

    int state_id0 = 0;
    int state_id1 = 5;

    imm_path_add(&path, state_id0, 0);
    imm_path_add(&path, state_id1, 0);

    imm_path_destroy(&path);
}
