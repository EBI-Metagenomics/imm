#ifndef MODEL1_H
#define MODEL1_H

#include "imm/imm.h"

struct model1
{
    struct imm_hmm*                hmm;
    struct imm_abc const*          abc;
    struct imm_vecp*               mute_states;
    struct imm_vecp*               normal_states;
    struct imm_normal_state const* end;
};

struct model1 create_model1(void);
void          destroy_model1(struct model1 model);
char const*   get_model1_str1(void);
char const*   get_model1_str2(void);

#endif
