#ifndef DP_IMM_DP_H
#define DP_IMM_DP_H

#include "dp/code.h"
#include "dp/emission.h"
#include "dp/state_table.h"
#include "dp/trans_table.h"

struct imm_dp
{
    struct code code;
    struct emission emission;
    struct trans_table trans_table;
    struct state_table state_table;
};

#endif
