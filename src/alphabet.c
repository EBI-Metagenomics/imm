#define NHMM_API_EXPORTS

#include "alphabet.h"
#include "nhmm.h"
#include <stdlib.h>

NHMM_API struct nhmm_alphabet *nhmm_create_alphabet(const char *abc)
{
    struct nhmm_alphabet *alphabet = malloc(sizeof(struct nhmm_alphabet));

    alphabet->abc = sdsnew(abc);

    return alphabet;
}

NHMM_API void nhmm_destroy_alphabet(struct nhmm_alphabet *alphabet) {
  if (!alphabet)
    return;

  if (alphabet->abc)
    sdsfree(alphabet->abc);
}
