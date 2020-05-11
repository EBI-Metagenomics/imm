#include "imm/input.h"
#include "free.h"
#include "imm/report.h"
#include "model.h"
#include <stdlib.h>
#include <string.h>

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
    input->status = 0;

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

struct imm_model const* imm_input_read(struct imm_input* input)
{
    struct imm_model* model = __imm_model_new(NULL);
    if (__imm_model_read(model, input->stream)) {
        imm_error("failed to read file %s", input->filepath);
        return NULL;
    }
    return model;
}
