#include "nhmm.h"
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
    struct nhmm_path *path;

    nhmm_path_create(&path);

    int state_id0 = 0;
    int state_id1 = 5;

    nhmm_path_add(&path, state_id0, 0);
    nhmm_path_add(&path, state_id1, 0);

    nhmm_path_destroy(&path);
}
