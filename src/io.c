#include "imm/io.h"
#include "dp.h"
#include "hmm.h"
#include "imm/report.h"

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

struct imm_io const* imm_io_read(FILE* stream) { return NULL; }
