#include "imm/path.h"
#include "error.h"
#include "imm/step.h"
#include "reallocf.h"
#include <assert.h>
#include <stdlib.h>

static enum imm_rc path_setup(struct imm_path *path)
{
    path->capacity = sizeof(*path->steps) * (1 << 4);
    path->nsteps = 0;
    path->dir = 1;
    path->start = 0;
    path->steps = malloc((size_t)path->capacity);
    if (!path->steps) return error(IMM_NOMEM);
    return IMM_OK;
}

enum imm_rc imm_path_add(struct imm_path *path, struct imm_step step)
{
    enum imm_rc rc = IMM_OK;
    if (path->capacity == 0)
    {
        if ((rc = path_setup(path))) return rc;
    }

    size_t count = (unsigned)path->nsteps + 1;
    size_t capacity = (size_t)path->capacity;

    if (sizeof *path->steps * count > capacity)
    {
        capacity <<= 1;
        assert(capacity >= sizeof *path->steps * count);
        path->steps = imm_reallocf(path->steps, capacity);
        if (!path->steps && capacity > 0) return error(IMM_NOMEM);
    }

    path->capacity = (int)capacity;
    imm_path_add_unsafe(path, step);
    return rc;
}

void imm_path_del(struct imm_path *path)
{
    if (path)
    {
        free(path->steps);
    }
}
