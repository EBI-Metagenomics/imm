#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "imm/export.h"
#include "imm/step.h"
#include <stdint.h>

struct imm_path
{
    unsigned long size;
    uint32_t nsteps;
    struct imm_step *steps;
};

IMM_API void imm_path_add(struct imm_path *path, struct imm_step step);

static inline void imm_path_add_unsafe(struct imm_path *path,
                                       struct imm_step step)
{
    path->steps[path->nsteps++] = step;
}

IMM_API struct imm_path *imm_path_new(void);

IMM_API void imm_path_del(struct imm_path const *path);

static inline void imm_path_reset(struct imm_path *path) { path->nsteps = 0; }

static inline uint32_t imm_path_nsteps(struct imm_path const *path)
{
    return path->nsteps;
}

static inline struct imm_step *imm_path_step(struct imm_path const *path,
                                             uint32_t step)
{
    return path->steps + step;
}

#endif
