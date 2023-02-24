#ifndef _FMT_ERROR_H
#define _FMT_ERROR_H

#define WAVE_ERROR "Error"
#define WAVE_WARN "Warn"
#define WAVE_DEBUG "Debug"
#define WAVE_PANIC "Panic"

void fmt_error(char *level, char *reason);
void fmt_panic(char *reason);

#endif /* _FMT_ERROR_H */
