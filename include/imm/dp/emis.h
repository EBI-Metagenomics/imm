#ifndef IMM_DP_EMIS_H
#define IMM_DP_EMIS_H

#include "imm/float.h"
#include <stdint.h>

struct imm_dp_emis
{
    imm_float *score; /**< Sequence emission score of a state. */
    uint32_t *offset; /**< Maps state to score array offset. */
};

#endif
