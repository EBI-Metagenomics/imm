#ifndef DP_PREMISE_H
#define DP_PREMISE_H

#include "dp/state_type.h"
#include <stdbool.h>

struct premise
{
    enum state_type state;
    bool first_row;
    bool safe_past;
    bool safe_floor;
    bool safe_ceiling;
};

#endif
