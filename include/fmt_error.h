#ifndef _FMT_ERROR_H
#define _FMT_ERROR_H

#define WAVE_ERROR "Error"
#define WAVE_WARN "Warn"
#define WAVE_DEBUG "Debug"
#define WAVE_PANIC "Panic"

void fmt_error(char *level, char *when);
void fmt_panic(char *when);

#endif /* _FMT_ERROR_H */