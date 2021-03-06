#include "imm/imm.h"
#include "profile.h"
#include "std.h"
#include <stdlib.h>
#include <string.h>

struct imm_input
{
    FILE*       stream;
    char const* filepath;

    bool eof;
    bool closed;
};

static struct imm_profile const* read_profile(struct imm_input* input, uint8_t block_type);

int imm_input_close(struct imm_input* input)
{
    if (input->closed)
        return 0;

    int errno = 0;

    if (fclose(input->stream)) {
        error("failed to close file %s", input->filepath);
        errno = 1;
    }

    input->closed = true;
    return errno;
}

struct imm_input* imm_input_create(char const* filepath)
{
    FILE* stream = fopen(filepath, "r");
    if (!stream) {
        error("could not open file %s for reading", filepath);
        return NULL;
    }

    struct imm_input* input = xmalloc(sizeof(*input));
    input->stream = stream;
    input->filepath = xstrdup(filepath);
    input->eof = false;
    input->closed = false;

    return input;
}

int imm_input_destroy(struct imm_input* input)
{
    int errno = imm_input_close(input);
    free((void*)input->filepath);
    free(input);
    return errno;
}

bool imm_input_eof(struct imm_input const* input) { return input->eof; }

struct imm_profile const* imm_input_read(struct imm_input* input)
{
    uint8_t block_type = 0x00;

    if (fread(&block_type, sizeof(block_type), 1, input->stream) < 1) {
        error("could not read block type");
        return NULL;
    }

    return read_profile(input, block_type);
}

static struct imm_profile const* read_profile(struct imm_input* input, uint8_t block_type)
{
    if (block_type == IMM_IO_BLOCK_EOF) {
        input->eof = true;
        return NULL;
    }

    if (block_type != IMM_IO_BLOCK_MODEL) {
        error("unknown block type");
        return NULL;
    }

    struct imm_profile const* prof = NULL;
    if (!(prof = imm_profile_read(input->stream))) {
        error("failed to read file %s", input->filepath);
        return NULL;
    }
    return prof;
}
