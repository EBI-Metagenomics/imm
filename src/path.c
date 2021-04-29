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

void imm_path_init(struct imm_path *path)
{
    path->capacity = sizeof(*path->steps) * (1 << 4);
    path->nsteps = 0;
    path->steps = xmalloc(path->capacity);
}

void imm_path_deinit(struct imm_path *path) { free(path->steps); }
