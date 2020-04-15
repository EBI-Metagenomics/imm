#include "imm/hmm.h"
#include "imm/abc.h"
#include "imm/io.h"
#include "io.h"
#include "dp.h"
#include "hmm.h"
#include "imm/report.h"
#include "free.h"
#include <stdlib.h>

int imm_io_write(FILE* stream, struct imm_hmm const* hmm, struct imm_dp const* dp)
{
    if (hmm_write(hmm, dp, stream)) {
        imm_error("could not write hmm");
        return 1;
    }

    if (dp_write(dp, stream)) {
        imm_error("could not write dp");
        return 1;
    }

    return 0;
}

struct imm_io const* imm_io_read(FILE* stream)
{
    struct imm_io *io = malloc(sizeof(*io));

    if (!hmm_read(stream, io)) {
        imm_error("could not read hmm");
        goto err;
    }

    return io;

err:
    if (io->abc)
        imm_abc_destroy(io->abc);

    if (io->hmm)
        imm_hmm_destroy(io->hmm);

    if (io->mstates)
        free_c(io->mstates);

    free_c(io);
    return NULL;
}
