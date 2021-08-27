#ifndef IMM_DP_EMIS_H
#define IMM_DP_EMIS_H

#include "imm/float.h"

struct imm_dp_emis
{
    imm_float *score; /**< Sequence emission score of a state. */
    unsigned *offset; /**< Maps state to score array offset. */
};

#endif
