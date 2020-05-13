#include "imm/input.h"
#include "free.h"
#include "imm/io.h"
#include "imm/report.h"
#include "model.h"
#include <stdlib.h>
#include <string.h>

static struct imm_model const* read_block(struct imm_input* input, uint8_t block_type);

struct imm_input_vtable imm_input_vtable = {read_block};

struct imm_input* imm_input_create(char const* filepath)
{
    FILE* stream = fopen(filepath, "r");
    if (!stream) {
        imm_error("could not open file %s for reading", filepath);
        return NULL;
    }

    struct imm_input* input = malloc(sizeof(*input));
    input->stream = stream;
    input->filepath = strdup(filepath);
    input->eof = false;
    input->vtable = imm_input_vtable;

    return input;
}

int imm_input_destroy(struct imm_input const* input)
{
    if (fclose(input->stream)) {
        imm_error("failed to close file %s", input->filepath);
        return 1;
    }
    free_c(input->filepath);
    free_c(input);
    return 0;
}

bool imm_input_eof(struct imm_input const* input) { return input->eof; }

struct imm_model const* imm_input_read(struct imm_input* input)
{
    uint8_t block_type = 0x00;

    if (fread(&block_type, sizeof(block_type), 1, input->stream) < 1) {
        imm_error("could not read block type");
        return NULL;
    }

    return input->vtable.read_block(input, block_type);
}

static struct imm_model const* read_block(struct imm_input* input, uint8_t block_type)
{
    if (block_type == IMM_IO_BLOCK_EOF) {
        input->eof = true;
        return NULL;
    }

    if (block_type != IMM_IO_BLOCK_MODEL) {
        imm_error("unknown block type");
        return NULL;
    }

    struct imm_model* model = __imm_model_new(NULL);
    if (imm_model_read(model, input->stream)) {
        imm_error("failed to read file %s", input->filepath);
        return NULL;
    }
    return model;
}
