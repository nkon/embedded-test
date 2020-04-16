#include <stdarg.h>

extern void xputchar(char c);

static void xitoa(int i, int base)
{
    int d, x;
    if (i < 0) {
        xputchar('-');
        i = -i;
    }
    if (base == 10) {
        x = 1000000000;
    } else if (base == 16) {
        x = 0xffffffff;
    }
    while (i < x) {
        x = x / base;
    }
    while (1) {
        d = i / x;
        xputchar(d + '0');
        i = i - d * x;
        x = x / base;
        if (!x) {
            break;
        }
    }
}

void xprintf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    const char *cp = fmt;
    char c;
    for (;;) {
        c = *cp++;
        if (!c) {
            break;
        } else if (c == '%') {
            c = *cp++;
            if (c == 'd') {
                xitoa(va_arg(list, int), 10);
            } else if (c == 's') {
                xprintf(va_arg(list, char *));
            } else if (c == 'c') {
                xputchar(va_arg(list, char));
            } else if (c == '%') {
                xputchar('%');
            }
        } else {
            xputchar(c);
        }
    }
    va_end(list);
}
