#ifndef IMM_PATH_H
#define IMM_PATH_H

#include "imm/api.h"

struct imm_path;
struct imm_state;
struct imm_step;

IMM_API struct imm_path* imm_path_create(void);
IMM_API void             imm_path_destroy(struct imm_path* path);
IMM_API int imm_path_append(struct imm_path* path, struct imm_state const* state, int seq_len);
IMM_API int imm_path_prepend(struct imm_path* path, struct imm_state const* state,
                             int seq_len);
IMM_API struct imm_step const* imm_path_first(struct imm_path const* path);
IMM_API struct imm_step const* imm_path_last(struct imm_path const* path);
IMM_API struct imm_step const* imm_path_next(struct imm_path const* path,
                                             struct imm_step const* step);

#endif
