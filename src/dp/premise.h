#ifndef DP_PREMISE_H
#define DP_PREMISE_H

#include <stdbool.h>

struct premise
{
    bool safe_past;
    bool safe_future;
};

static inline struct premise premise_init(bool safe_past, bool safe_future)
{
    return (struct premise){safe_past, safe_future};
}

#endif
