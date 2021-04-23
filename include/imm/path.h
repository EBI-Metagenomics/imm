#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "imm/export.h"
#include "imm/step.h"

struct imm_path
{
    unsigned long size;
    uint32_t nsteps;
    struct imm_step *steps;
};

static inline void imm_path_add(struct imm_path *path, struct imm_step step)
{
    path->steps[path->nsteps++] = step;
}

IMM_API void imm_path_add_safe(struct imm_path *path, struct imm_step step);

IMM_API struct imm_path *imm_path_create(void);

IMM_API void imm_path_del(struct imm_path const *path);

static inline void imm_path_reset(struct imm_path *path) { path->nsteps = 0; }

static inline struct imm_step *imm_path_step(struct imm_path *path,
                                             uint32_t step)
{
    return path->steps + step;
}

#endif
