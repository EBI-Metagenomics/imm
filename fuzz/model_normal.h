#ifndef TEST_MODEL_NORMAL_H
#define TEST_MODEL_NORMAL_H

#include "imm/imm.h"

#define MODEL_NORMAL_SIZE 1000U

void model_normal_init(void);
void model_normal_deinit(void);

struct model_normal
{
    struct imm_abc abc;
    struct imm_hmm *hmm;
    struct imm_mute_state *start;
    struct imm_normal_state *b;
    struct imm_normal_state *j;
    struct imm_normal_state *m[MODEL_NORMAL_SIZE];
    struct imm_normal_state *i[MODEL_NORMAL_SIZE];
    struct imm_mute_state *d[MODEL_NORMAL_SIZE];
    struct imm_normal_state *e;
    struct imm_mute_state *end;
};

extern char const model_normal_str[];
extern struct model_normal model_normal;

#endif
