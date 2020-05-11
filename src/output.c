#include "imm/output.h"
#include "free.h"
#include "imm/io.h"
#include "imm/report.h"
#include "model.h"
#include <stdlib.h>
#include <string.h>

struct imm_output* imm_output_create(char const* filepath)
{
    FILE* stream = fopen(filepath, "w");
    if (!stream) {
        imm_error("could not open file %s for writing", filepath);
        return NULL;
    }

    struct imm_output* io = malloc(sizeof(*io));
    io->stream = stream;
    io->filepath = strdup(filepath);

    return io;
}

int imm_output_destroy(struct imm_output const* output)
{
    uint8_t block_type = IMM_IO_BLOCK_EOF;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        imm_error("could not write block type");
        return 1;
    }

    if (fclose(output->stream)) {
        imm_error("failed to close file %s", output->filepath);
        return 1;
    }
    free_c(output->filepath);
    free_c(output);
    return 0;
}

int imm_output_write(struct imm_output* output, struct imm_model const* model)
{
    uint8_t block_type = IMM_IO_BLOCK_MODEL;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        imm_error("could not write block type");
        return 1;
    }

    if (model->vtable.write(model, output->stream)) {
        imm_error("could not write io entry");
        return 1;
    }
    return 0;
}
