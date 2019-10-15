#ifndef IMM_PATH_H_API
#define IMM_PATH_H_API

#include "imm/api.h"

struct imm_path;
struct imm_state;

IMM_API struct imm_path *imm_path_create(void);
IMM_API void imm_path_destroy(struct imm_path *path);
IMM_API void imm_path_add(struct imm_path *path, const struct imm_state *state, int seq_len);

#endif
