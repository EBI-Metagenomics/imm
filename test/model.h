#ifndef MODEL_H
#define MODEL_H

#include "imm/imm.h"

struct model
{
    struct imm_hmm*              hmm;
    struct imm_abc const*        abc;
    struct imm_vecp*             mute_states;
    struct imm_vecp*             normal_states;
    struct imm_mute_state const* end;
};

struct model create_model(void);
void         destroy_model(struct model model);
char const*  get_model_str(void);

#endif
