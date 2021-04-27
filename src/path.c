#include "imm/path.h"
#include "common/common.h"
#include "imm/step.h"

void imm_path_add(struct imm_path *path, struct imm_step step)
{
    size_t size = sizeof(*path->steps);
    size_t count = path->nsteps + 1;
    path->steps = growmem(path->steps, count, size, &path->capacity);
    imm_path_add_unsafe(path, step);
}

struct imm_path *imm_path_new(void)
{
    struct imm_path *path = xmalloc(sizeof(*path));
    path->capacity = sizeof(*path->steps) * (1 << 4);
    path->nsteps = 0;
    path->steps = xmalloc(path->capacity);
    return path;
}

void imm_path_del(struct imm_path const *path)
{
    free(path->steps);
    free((void *)path);
}
