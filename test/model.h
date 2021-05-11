#ifndef MODEL_H
#define MODEL_H

#include "imm/imm.h"

struct model
{
    char abc_str[6];
    struct imm_abc abc;
    imm_float B_lprobs[5];
    imm_float E_lprobs[5];
    imm_float J_lprobs[5];
    imm_float M_lprobs[5];
    imm_float I_lprobs[5];
    struct imm_hmm *hmm;
    struct imm_vecp *mute_states;
    struct imm_vecp *normal_states;
    struct imm_mute_state *end;
};

struct model create_model(void);
void destroy_model(struct model model);
char const *get_model_str(void);

#endif
