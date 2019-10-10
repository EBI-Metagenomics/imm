#ifndef IMM_PATH_H_API
#define IMM_PATH_H_API

#include "imm/api.h"

struct imm_path;
struct imm_state;

IMM_API void imm_path_create(struct imm_path **path_ptr);
IMM_API void imm_path_destroy(struct imm_path **path_ptr);
IMM_API void imm_path_add(struct imm_path **path_ptr, const struct imm_state *state,
                          int seq_len);

#endif
