#ifndef MODEL2_H
#define MODEL2_H

#include "imm/imm.h"

struct model2
{
    struct imm_hmm*              hmm;
    struct imm_abc const*        abc;
    struct imm_vecp*             mute_states;
    struct imm_vecp*             normal_states;
    struct imm_mute_state const* end;
};

struct model2 create_model2(void);
void          destroy_model2(struct model2 model);
char const*   get_model2_str(void);

#endif
