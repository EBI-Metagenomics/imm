#include "eseq.h"
#include "free.h"

void eseq_destroy(struct eseq const* eseq)
{
    matrixu_destroy(eseq->code);
    free_c(eseq);
}
