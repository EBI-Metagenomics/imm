#include "src/imm/counter.h"
#include "imm.h"
#include <stdlib.h>

struct counter
{
    int next_count;
};

struct counter* counter_create(void)
{
    struct counter* counter = malloc(sizeof(struct counter));
    counter->next_count = 0;
    return counter;
}

int counter_next(struct counter* counter)
{
    int next = counter->next_count++;
    if (next >= 0)
        return next;
    imm_error("counter overflow");
    return -1;
}

void counter_destroy(struct counter* counter)
{
    if (counter) {
        imm_error("counter should not be NULL");
        free(counter);
    }
}
