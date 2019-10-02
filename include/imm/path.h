#ifndef IMM_PATH_H_API
#define IMM_PATH_H_API

#include "imm/api.h"
#include <stddef.h>

struct imm_path;

IMM_API void imm_path_create(struct imm_path **path);
IMM_API void imm_path_add(struct imm_path **path, int state_id, size_t seq_len);
IMM_API void imm_path_destroy(struct imm_path **path);

#endif
