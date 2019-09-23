#ifndef NHMM_PATH_H
#define NHMM_PATH_H

#include <stddef.h>

struct nhmm_path
{
    int state_id;
    size_t seq_len;
    struct nhmm_path *next;
    struct nhmm_path *prev;
};

#endif
