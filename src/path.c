#include "imm/path.h"
#include "common/common.h"
#include "imm/step.h"
#include <stdlib.h>

static inline void grow_if_needed(struct imm_path *path)
{
    if (sizeof(struct imm_step) * (path->nsteps + 1) > path->size)
    {
        path->size <<= 1;
        path->steps = xrealloc(path->steps, path->size);
    }
}

void imm_path_add(struct imm_path *path, struct imm_step step)
{
    grow_if_needed(path);
    imm_path_add_unsafe(path, step);
}

struct imm_path *imm_path_new(void)
{
    struct imm_path *path = xmalloc(sizeof(*path));
    path->size = sizeof(struct imm_step) * (1 << 4);
    path->nsteps = 0;
    path->steps = xmalloc(path->size);
    return path;
}

void imm_path_del(struct imm_path const *path)
{
    free(path->steps);
    free((void *)path);
}
