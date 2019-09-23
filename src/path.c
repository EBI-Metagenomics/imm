#define NHMM_API_EXPORTS

#include "nhmm/path.h"
#include "path.h"
#include "utlist.h"
#include <stdlib.h>

NHMM_API void nhmm_path_create(struct nhmm_path **path) { *path = NULL; }

NHMM_API void nhmm_path_add(struct nhmm_path **path, int state_id, size_t seq_len)
{
    struct nhmm_path *elem = malloc(sizeof(struct nhmm_path));
    elem->state_id = state_id;
    elem->seq_len = seq_len;
    DL_APPEND(*path, elem);
}

NHMM_API void nhmm_path_destroy(struct nhmm_path **path)
{
    struct nhmm_path *elem, *tmp;
    DL_FOREACH_SAFE(*path, elem, tmp)
    {
        elem->state_id = NHMM_STATE_ID_INVALID;
        elem->seq_len = 0;
        DL_DELETE(*path, elem);
        free(elem);
    }
}
