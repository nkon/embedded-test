#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

extern void (* xputchar)(char);
extern void xprintf(const char *fmt, ...);

#endif // PRINTF_H
