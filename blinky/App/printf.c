#include <stdarg.h>
#include "assert.h"

// function pointer to the 1-char output function.
void (*xputchar)(char c);
extern void (*xputs)(char *str);

static void snputchar(char *buf, int len, char *ptr, char c)
{
    return;
}

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
        x = 0x10000000;
    }
    while (i < x) { // suppless leading '0'
        x = x / base;
    }
    while (1) {
        d = i / x;
        if (base == 10) {
            xputchar(d + '0');
        } else if (base == 16) {
            xputchar((d < 10) ? (d + '0') : (d - 10 + 'a'));
        }
        i = i - d * x;
        x = x / base;
        if (!x) {
            break;
        }
    }
}

int xsnprintf(char *buf, int len, const char *fmt,...)
{
    return 0;
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
            } else if (c == 'x') {
                xitoa(va_arg(list, int), 16);
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

#ifdef TEST

#include <string.h>

static char s_buf[256];
static char s_cnt = 0;

void buf_putchar(char c) { s_buf[s_cnt++] = c; }

void test_printf(void)
{
    xputchar = buf_putchar;

    // xprintf("printf") == "printf"
    s_cnt = 0;
    xprintf("printf");
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "printf"));

    // xprintf("d:%d x:%x", 10, 10) == "d:10 x:a"
    s_cnt = 0;
    xprintf("d:%d x:%x", 10, 10);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:10 x:a"));

    s_cnt = 0;
    xprintf("d:%d x:%x", 0xffff, 0xffff);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:65535 x:ffff"));

    s_cnt = 0;
    xprintf("d:%d x:%x", 0x7fffffff, 0x7fffffff);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:2147483647 x:7fffffff"));

    s_cnt = 0;
    xprintf("d:%d x:%x", 0xffffffff, 0xffffffff);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:-1 x:-1"));

    s_cnt = 0;
    xprintf("d:%d x:%x", -1, -1);
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "d:-1 x:-1"));

    s_cnt = 0;
    xprintf("Hello, %s!", "World");
    s_buf[s_cnt] = 0;
    assert(!strcmp(s_buf, "Hello, World!"));
}

#endif // TEST
