#include "assert.h"
#include <stdarg.h>

// function pointer to the 1-char output function.
void (*xputchar)(char c);
void (*xputs)(char *str);

static void snputchar(char *buf, int len, char *ptr, char c) { return; }

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

static int xsitoa(char *buf, int len, int i, int base)
{
    int d, x;
    int used = 0;

    if (i < 0) {
        buf[used++] = '-';
        i = -i;
    }
    if (base == 10) {
        x = 1000000000;
    } else if (base == 16) {
        x = 0x10000000;
    } else {
        assert(0);
    }
    while (i < x) { // suppless leading '0'
        x = x / base;
    }
    while (1) {
        d = i / x;
        if (base == 10) {
            buf[used++] = d + '0';
        } else if (base == 16) {
            buf[used++] = (d < 10) ? (d + '0') : (d - 10 + 'a');
        }
        i = i - d * x;
        x = x / base;
        if (!x) {
            break;
        }
        if (used >= len) {
            break;
        }
    }
    buf[used] = '\0';
    return used;
}

int xsnprintf(char *buf, int len, const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    const char *cp = fmt;
    char c;
    int used = 0;
    while (c = *cp++) {
        if (c == '%') {
            c = *cp++;
            if (c == 'd') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 10);
            } else if (c == 'x') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 16);
            } else if (c == 's') {
                used += xsnprintf(buf + used, len - used, va_arg(list, char *));
            } else if (c == 'c') {
                buf[used] = va_arg(list, char);
                used++;
            } else if (c == '%') {
                buf[used] = c;
                used++;
            }
        } else {
            buf[used++] = c;
        }
        if (len <= used) {
            return used;
        }
    }
    va_end(list);

    buf[used] = '\0';
    return used;
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
static int s_cnt = 0;

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

void test_snprintf(void)
{
    char buf[256];
    int ret;

    ret = xsnprintf(buf, 255, "printf");
    assert(ret == 6);
    assert(!strcmp(buf, "printf"));

    // xprintf("d:%d x:%x", 10, 10) == "d:10 x:a"
    xsnprintf(buf, 255, "d:%d x:%x", 10, 10);
    assert(!strcmp(buf, "d:10 x:a"));

    xsnprintf(buf, 255, "d:%d x:%x", 0xffff, 0xffff);
    assert(!strcmp(buf, "d:65535 x:ffff"));

    xsnprintf(buf, 255, "d:%d x:%x", 0x7fffffff, 0x7fffffff);
    assert(!strcmp(buf, "d:2147483647 x:7fffffff"));

    xsnprintf(buf, 255, "d:%d x:%x", 0xffffffff, 0xffffffff);
    assert(!strcmp(buf, "d:-1 x:-1"));

    xsnprintf(buf, 255, "d:%d x:%x", -1, -1);
    assert(!strcmp(buf, "d:-1 x:-1"));

    xsnprintf(buf, 255, "Hello, %s!", "World");
    assert(!strcmp(buf, "Hello, World!"));
}

#endif // TEST
