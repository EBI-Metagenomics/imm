#include "log/log.h"
#include "hope/hope.h"
#include "imm/imm.h"

void test_log_setup(void);
void test_log_with_third_party_lib(void);

int main(void)
{
    test_log_setup();
    test_log_with_third_party_lib();
    return hope_status();
}

static void print_fprintf(char const *msg, void *arg)
{
    FILE *fd = arg;
    fprintf(fd, "PRINT_FPRINTF: %s", msg);
}

void test_log_setup(void)
{
    FILE *fd = fopen(TMPDIR "/output.txt", "w");
    NOTNULL(fd);
    imm_log_setup(print_fprintf, fd);

    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_ILLEGALARG);

    fclose(fd);

    fd = fopen(TMPDIR "/output.txt", "r");
    NOTNULL(fd);

    char actual[256];
    NOTNULL(fgets(actual, sizeof actual, fd));
    fclose(fd);

    char desired[] =
        "PRINT_FPRINTF: /Users/horta/code/imm/src/abc.c:70:IMM_ILLEGALARG: "
        "alphabet "
        "cannot have duplicated symbols";

    char *desired_ctx = NULL;
    char *desired_tok = strtok_r(desired, ":", &desired_ctx);

    char *line_ctx = NULL;
    char *line_tok = strtok_r(actual, ":", &line_ctx);

    EQ(line_tok, desired_tok);

    strtok_r(NULL, ":", &desired_ctx);
    strtok_r(NULL, ":", &line_ctx);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    /* C language: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2322.htm */
    /* So we skip the __LINE__ check as its value varies across compilers */
    /* EQ(line_tok, desired_tok); */

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);
}

static void print_log_put(char const *msg, void *arg)
{
    __log_put(LOG_ERROR, msg);
}

static void flush_fflush(void *arg)
{
    FILE *fd = arg;
    fflush(fd);
}

void test_log_with_third_party_lib(void)
{
    FILE *fd = fopen(TMPDIR "/output_third_party.txt", "w");
    NOTNULL(fd);

    imm_log_setup(print_log_put, NULL);
    log_setup(LOG_ERROR, print_fprintf, flush_fflush, fd);

    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_ILLEGALARG);

    log_flush();
    fclose(fd);

    fd = fopen(TMPDIR "/output_third_party.txt", "r");
    NOTNULL(fd);

    char actual[256];
    NOTNULL(fgets(actual, sizeof actual, fd));
    fclose(fd);

    char desired[] =
        "PRINT_FPRINTF: /Users/horta/code/imm/src/abc.c:70:IMM_ILLEGALARG: "
        "alphabet cannot have duplicated symbols";

    char *desired_ctx = NULL;
    char *desired_tok = strtok_r(desired, ":", &desired_ctx);

    char *line_ctx = NULL;
    char *line_tok = strtok_r(actual, ":", &line_ctx);

    strtok_r(NULL, ":", &desired_ctx);
    strtok_r(NULL, ":", &line_ctx);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);
}
