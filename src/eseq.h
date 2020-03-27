#ifndef IMM_ESEQ_H
#define IMM_ESEQ_H

#include "seq_code.h"
#include "matrixu.h"

struct eseq
{
    struct matrixu*            code;
    struct seq_code const* seq_code;
};

void eseq_destroy(struct eseq const* eseq);

#endif
