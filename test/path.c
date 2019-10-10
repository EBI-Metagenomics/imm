#include "imm.h"
#include "cass/cass.h"

void test_path(void);

int main(void)
{
    test_path();
    return cass_status();
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
