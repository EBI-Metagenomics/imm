#include "imm/output.h"
#include "free.h"
#include "imm/io.h"
#include "imm/report.h"
#include "model.h"
#include <stdlib.h>
#include <string.h>

struct imm_output
{
    FILE*       stream;
    char const* filepath;
};

struct imm_output* imm_output_create(char const* filepath)
{
    FILE* stream = fopen(filepath, "w");
    if (!stream) {
        imm_error("could not open file %s for writing", filepath);
        return NULL;
    }

    struct imm_output* output = malloc(sizeof(*output));
    output->stream = stream;
    output->filepath = strdup(filepath);

    return output;
}

int imm_output_destroy(struct imm_output const* output)
{
    uint8_t block_type = IMM_IO_BLOCK_EOF;
    int     errno = 0;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        imm_error("could not write block type");
        errno = 1;
    }

    if (fclose(output->stream)) {
        imm_error("failed to close file %s", output->filepath);
        errno = 1;
    }
    free_c(output->filepath);
    free_c(output);
    return errno;
}

int imm_output_write(struct imm_output* output, struct imm_model const* model)
{
    uint8_t block_type = IMM_IO_BLOCK_MODEL;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        imm_error("could not write block type");
        return 1;
    }

    if (imm_model_write(model, output->stream)) {
        imm_error("could not write imm model");
        return 1;
    }
    return 0;
}
