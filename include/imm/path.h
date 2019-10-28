#ifndef IMM_PATH_H_API
#define IMM_PATH_H_API

#include "imm/api.h"

struct imm_path;
struct imm_state;
struct imm_step;

IMM_API struct imm_path* imm_path_create(void);
IMM_API void             imm_path_destroy(struct imm_path* path);
IMM_API int imm_path_add(struct imm_path* path, const struct imm_state* state, int seq_len);
IMM_API struct imm_step const*  imm_path_first(struct imm_path const* path);
IMM_API struct imm_step const*  imm_path_next(struct imm_path const* path,
                                              struct imm_step const* step);
IMM_API const struct imm_state* imm_step_state(struct imm_step const* step);
IMM_API int                     imm_step_seq_len(struct imm_step const* step);

#endif
