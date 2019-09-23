#define NHMM_API_EXPORTS

#include "nhmm/path.h"
#include "utlist.h"
#include <stdlib.h>

struct nhmm_path
{
    const struct nhmm_state *state;
    size_t seq_len;
    struct nhmm_path *next;
    struct nhmm_path *prev;
};

NHMM_API void nhmm_path_create(struct nhmm_path **path) { *path = NULL; }

NHMM_API void nhmm_path_add(struct nhmm_path **path, struct nhmm_state *state,
                            size_t seq_len)
{
    struct nhmm_path *elem = malloc(sizeof(struct nhmm_path));
    elem->state = state;
    elem->seq_len = seq_len;
    DL_APPEND(*path, elem);
}

NHMM_API void nhmm_path_destroy(struct nhmm_path **path)
{
    struct nhmm_path *elem, *tmp;
    DL_FOREACH_SAFE(*path, elem, tmp)
    {
        elem->state = NULL;
        elem->seq_len = 0;
        DL_DELETE(*path, elem);
        free(elem);
    }
}
