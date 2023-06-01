#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "export.h"
#include "state.h"
#include "step.h"
#include <stddef.h>
#include <stdio.h>

struct imm_path
{
  int capacity;
  int nsteps;
  int dir;
  int start;
  struct imm_step *steps;
};

IMM_API struct imm_path imm_path(void);
IMM_API int imm_path_add(struct imm_path *, struct imm_step);
IMM_API struct imm_step *imm_path_step(struct imm_path const *, unsigned);
IMM_API void imm_path_add_unsafe(struct imm_path *, struct imm_step);
IMM_API void imm_path_cleanup(struct imm_path *);
IMM_API void imm_path_reset(struct imm_path *);
IMM_API unsigned imm_path_nsteps(struct imm_path const *);
IMM_API void imm_path_reverse(struct imm_path *);
IMM_API void imm_path_dump(struct imm_path const *, imm_state_name *,
                           struct imm_seq const *, FILE *restrict);

#endif
