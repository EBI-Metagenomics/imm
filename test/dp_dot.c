#include "hope/hope.h"
#include "imm/imm.h"

static void state_name(unsigned id, char name[IMM_STATE_NAME_SIZE])
{
    name[0] = 'M';
    if (id == 0)
        name[1] = '0';
    else
        name[1] = '1';
    name[2] = '\0';
}

int main(void)
{
    struct imm_abc abc;
    imm_abc_init(&abc, IMM_STR("ACGT"), '*');

    struct imm_mute_state state0;
    imm_mute_state_init(&state0, 0, &abc);
    struct imm_mute_state state1;
    imm_mute_state_init(&state1, 1, &abc);
    struct imm_hmm hmm;
    imm_hmm_init(&hmm, &abc);

    EQ(imm_hmm_add_state(&hmm, imm_super(&state0)), IMM_SUCCESS);
    EQ(imm_hmm_add_state(&hmm, imm_super(&state1)), IMM_SUCCESS);

    EQ(imm_hmm_set_trans(&hmm, imm_super(&state0), imm_super(&state1),
                         imm_log(0.5)),
       IMM_SUCCESS);

    EQ(imm_hmm_set_start(&hmm, imm_super(&state0), 0.5f), IMM_SUCCESS);

    FILE *fd = fopen(TMPDIR "/hmm.dot", "w");
    NOTNULL(fd);
    imm_hmm_write_dot(&hmm, fd, state_name);
    fclose(fd);

    struct imm_dp dp;
    EQ(imm_hmm_init_dp(&hmm, imm_super(&state1), &dp), IMM_SUCCESS);

    fd = fopen(TMPDIR "/dp.dot", "w");
    NOTNULL(fd);
    imm_dp_write_dot(&dp, fd, state_name);
    fclose(fd);
    imm_del(&dp);

    FILE *a = fopen(TMPDIR "/hmm.dot", "r");
    NOTNULL(a);
    FILE *b = fopen(TMPDIR "/dp.dot", "r");
    NOTNULL(b);

    EQ(a, b);

    fclose(a);
    fclose(b);

    return hope_status();
}
