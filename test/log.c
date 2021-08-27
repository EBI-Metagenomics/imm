#include "log/log.h"
#include "hope/hope.h"
#include "imm/imm.h"

void test_log_setup(void);
void test_log_third_party(void);

int main(void)
{
    test_log_setup();
    test_log_third_party();
    return hope_status();
}

static void my_error(char const *msg, void *arg)
{
    FILE *fd = arg;
    fprintf(fd, "MY_ERROR: %s", msg);
}

static void my_fatal(char const *msg, void *arg)
{
    FILE *fd = arg;
    fprintf(fd, "MY_FATAL: %s", msg);
}

static void my_flush(void *arg)
{
    FILE *fd = arg;
    fflush(fd);
}

void test_log_setup(void)
{
    FILE *fd = fopen(TMPDIR "/output.txt", "w");
    NOTNULL(fd);
    imm_log_setup(my_error, my_fatal, my_flush, fd);

    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_ILLEGALARG);

    fclose(fd);

    fd = fopen(TMPDIR "/output.txt", "r");
    NOTNULL(fd);

    char actual[256];
    NOTNULL(fgets(actual, sizeof actual, fd));
    fclose(fd);

    char desired[] =
        "MY_ERROR: /Users/horta/code/imm/src/abc.c:72:IMM_ILLEGALARG: alphabet "
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
    /* C lang bug: http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2322.htm */
    /* EQ(line_tok, desired_tok); */

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);

    desired_tok = strtok_r(NULL, ":", &desired_ctx);
    line_tok = strtok_r(NULL, ":", &line_ctx);
    EQ(line_tok, desired_tok);
}

static void tp_error(char const *msg, void *arg) { __log_put(LOG_ERROR, msg); }

static void tp_fatal(char const *msg, void *arg) { __log_put(LOG_FATAL, msg); }

static void tp_flush(void *arg) { log_flush(); }

static void print_to_file(char const *msg, void *arg)
{
    FILE *fd = arg;
    fprintf(fd, "%s", msg);
}

static void flush_file(void *arg)
{
    FILE *fd = arg;
    fflush(fd);
}

void test_log_third_party(void)
{
    FILE *fd = fopen(TMPDIR "/output_third_party.txt", "w");
    NOTNULL(fd);

    imm_log_setup(tp_error, tp_fatal, tp_flush, NULL);
    log_setup(LOG_ERROR, print_to_file, flush_file, fd);

    struct imm_abc abc = imm_abc_empty;
    EQ(imm_abc_init(&abc, IMM_STR("ACTC"), '*'), IMM_ILLEGALARG);

    log_flush();
    fclose(fd);

    fd = fopen(TMPDIR "/output_third_party.txt", "r");
    NOTNULL(fd);

    char actual[256];
    NOTNULL(fgets(actual, sizeof actual, fd));
    fclose(fd);

    char desired[] = "/Users/horta/code/imm/src/abc.c:72:IMM_ILLEGALARG: "
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
}
