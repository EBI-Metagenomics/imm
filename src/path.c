#include "nhmm/path.h"

struct nhmm_path
{
int len;
};

NHMM_API struct nhmm_path *nhmm_path_create(void);
NHMM_API void nhmm_path_add(struct nhmm_path *path, struct nhmm_state *state, int len);
NHMM_API void nhmm_path_destroy(struct nhmm_path *path);
