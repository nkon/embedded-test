#ifndef PRINTF_H
#define PRINTF_H

#include <stdarg.h>

extern void (*xputs)(char *str);
extern int xsnprintf(char *buf, int len, const char *fmt, ...);
extern void xprintf(const char *fmt, ...);

#ifdef TEST
void test_printf(void);
void test_snprintf(void);
#endif // TEST

#endif // PRINTF_H
