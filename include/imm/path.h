#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "imm/export.h"
#include "imm/step.h"
#include <stddef.h>

struct imm_path
{
    size_t capacity;
    unsigned nsteps;
    struct imm_step *steps;
};

IMM_API void imm_path_add(struct imm_path *path, struct imm_step step);

static inline void imm_path_add_unsafe(struct imm_path *path,
                                       struct imm_step step)
{
    path->steps[path->nsteps++] = step;
}

IMM_API void imm_path_init(struct imm_path *path);

IMM_API void imm_path_deinit(struct imm_path *path);

static inline void imm_path_reset(struct imm_path *path) { path->nsteps = 0; }

static inline unsigned imm_path_nsteps(struct imm_path const *path)
{
    return path->nsteps;
}

static inline struct imm_step *imm_path_step(struct imm_path const *path,
                                             unsigned step)
{
    return path->steps + step;
}

#endif
