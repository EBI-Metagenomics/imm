#ifndef NHMM_PATH_H_API
#define NHMM_PATH_H_API

#include "nhmm/api.h"
#include <stddef.h>

struct nhmm_path;

NHMM_API void nhmm_path_create(struct nhmm_path **path);
NHMM_API void nhmm_path_add(struct nhmm_path **path, int state_id, size_t seq_len);
NHMM_API void nhmm_path_destroy(struct nhmm_path **path);

#endif
