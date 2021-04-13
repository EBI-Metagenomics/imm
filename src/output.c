#include "imm/imm.h"
#include "profile.h"
#include "std.h"
#include <stdlib.h>

struct imm_output
{
    FILE*       stream;
    char const* filepath;
    bool        closed;
};

int imm_output_close(struct imm_output* output)
{
    if (output->closed)
        return 0;

    uint8_t block_type = IMM_IO_BLOCK_EOF;
    int     errno = 0;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        error("could not write block type");
        errno = 1;
    }

    if (fclose(output->stream)) {
        error("failed to close file %s", output->filepath);
        errno = 1;
    }

    output->closed = true;
    return errno;
}

struct imm_output* imm_output_create(char const* filepath)
{
    FILE* stream = fopen(filepath, "w");
    if (!stream) {
        error("could not open file %s for writing", filepath);
        return NULL;
    }

    struct imm_output* output = xmalloc(sizeof(*output));
    output->stream = stream;
    output->filepath = xstrdup(filepath);
    output->closed = false;

    return output;
}

int imm_output_destroy(struct imm_output* output)
{
    int errno = imm_output_close(output);
    free((void*)output->filepath);
    free(output);
    return errno;
}

int imm_output_write(struct imm_output* output, struct imm_profile const* prof)
{
    uint8_t block_type = IMM_IO_BLOCK_MODEL;

    if (fwrite(&block_type, sizeof(block_type), 1, output->stream) < 1) {
        error("could not write block type");
        return 1;
    }

    if (imm_profile_write(prof, output->stream)) {
        error("could not write imm model");
        return 1;
    }
    return 0;
}
