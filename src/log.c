#include "imm/log.h"
#include "imm/support.h"
#include "support.h"

static char const *__msg[] = {
    [IMM_SUCCESS] = "success",          [IMM_FAILURE] = "failure",
    [IMM_OUTOFMEM] = "out of memory",   [IMM_ILLEGALARG] = "illegal argument",
    [IMM_IOERROR] = "i/o error",        [IMM_PARSEERROR] = "parse error",
    [IMM_RUNTIMEERROR] "runtime error",
};

static imm_log_callback log_callback = imm_log_default_callback;
static enum imm_level log_level = IMM_WARN;

void imm_log_setup(imm_log_callback cb, enum imm_level level)
{
    log_callback = cb;
    log_level = level;
}

enum imm_rc __imm_log_impl(enum imm_level level, enum imm_rc rc,
                           char const *file, int line, char const *fmt, ...)
{
    if (level < log_level)
        return rc;

    struct imm_log_event e = {
        .fmt = fmt, .file = file, .line = line, .level = level, .rc = rc};
    va_start(e.va, fmt);
    log_callback(e);
    va_end(e.va);

    if (imm_unlikely(level == IMM_FATAL))
        exit(EXIT_FAILURE);

    return rc;
}

void imm_log_default_callback(struct imm_log_event event)
{
    fprintf(stderr, "%s:%d:%s: ", event.file, event.line, __msg[event.rc]);
    vfprintf(stderr, event.fmt, event.va);
    fputc('\n', stderr);
}

void __imm_bug(char const *file, int line, char const *cond)
{
    fprintf(stderr, "BUG: %s:%d: %s\n", file, line, cond);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
