#include "imm/path.h"
#include "common/common.h"
#include "imm/step.h"

void imm_path_add(struct imm_path *path, struct imm_step step)
{
    size_t count = (unsigned)path->nsteps + 1;
    size_t capacity = (size_t)path->capacity;
    path->steps = growmem(path->steps, count, sizeof(*path->steps), &capacity);
    path->capacity = (int)capacity;
    imm_path_add_unsafe(path, step);
}

void imm_path_init(struct imm_path *path)
{
    path->capacity = sizeof(*path->steps) * (1 << 4);
    path->nsteps = 0;
    path->dir = 1;
    path->start = 0;
    path->steps = xmalloc((size_t)path->capacity);
}

void imm_path_deinit(struct imm_path *path) { free(path->steps); }
