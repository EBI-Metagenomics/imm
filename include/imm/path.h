#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "imm/export.h"
#include "imm/step.h"

struct imm_path
{
    int capacity;
    int nsteps;
    int dir;
    int start;
    struct imm_step *steps;
};

#define IMM_PATH_INIT()                                                        \
    {                                                                          \
        0, 0, 1, 0, NULL,                                                      \
    }

IMM_API void imm_path_add(struct imm_path *path, struct imm_step step);

static inline struct imm_step *imm_path_step(struct imm_path const *path,
                                             unsigned step)
{
    int i = path->start + (int)step * path->dir;
    int n = path->capacity;
    return path->steps + (unsigned)(((i % n) + n) % n);
}

static inline void imm_path_add_unsafe(struct imm_path *path,
                                       struct imm_step step)
{
    *imm_path_step(path, (unsigned)path->nsteps++) = step;
}

IMM_API void imm_path_init(struct imm_path *path);

IMM_API void imm_path_deinit(struct imm_path *path);

static inline void imm_path_reset(struct imm_path *path)
{
    path->nsteps = 0;
    path->dir = 1;
    path->start = 0;
}

static inline unsigned imm_path_nsteps(struct imm_path const *path)
{
    return (unsigned)path->nsteps;
}

static inline void imm_path_reverse(struct imm_path *path)
{
    if (path->nsteps == 0)
        return;
    int i = path->start + (int)path->nsteps * path->dir;
    int n = path->capacity;
    path->start = (((i % n) + n) % n) - 1;
    path->dir *= -1;
}

#endif
