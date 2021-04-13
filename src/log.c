#include "log.h"

static void default_print(struct imm_log_event event);

static imm_log_callback log_callback = default_print;
enum imm_log_level      log_level = IMM_LOG_WARN;

void imm_log_setup(imm_log_callback cb, enum imm_log_level level)
{
    log_callback = cb;
    log_level = level;
}

void log_log(enum imm_log_level level, char const* file, int line, char const* fmt, ...)
{
    if (level < log_level)
        return;

    struct imm_log_event e = {.fmt = fmt, .file = file, .line = line, .level = level};
    va_start(e.va, fmt);
    log_callback(e);
    va_end(e.va);
}

static void default_print(struct imm_log_event event)
{
    vfprintf(stderr, event.fmt, event.va);
    fputc('\n', stderr);
}
