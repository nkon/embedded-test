#include "assert.h"
#include <stdarg.h>

// function pointer to the string output function.
void (*xputs)(char *str);

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
    do {
        d = i / x;
        if (base == 10) {
            buf[used++] = d + '0';
        } else if (base == 16) {
            buf[used++] = (d < 10) ? (d + '0') : (d - 10 + 'a');
        }
        if (len <= used) {
            break;
        }
        i = i - d * x;
        x = x / base;
    } while (x);

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
    while ((c = *cp++)) {
        if (c == '%') {
            c = *cp++;
            if (c == 'd') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 10);
            } else if (c == 'x') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 16);
            } else if (c == 's') {
                used += xsnprintf(buf + used, len - used, va_arg(list, char *));
            } else if (c == 'c') {
                buf[used] = va_arg(list, int);
                used++;
            } else if (c == '%') {
                buf[used] = c;
                used++;
            }
        } else {
            buf[used++] = c;
        }
        if (len <= used) {
            buf[len - 1] = '\0';
            return used;
        }
    }
    va_end(list);

    buf[used] = '\0';
    return used;
}

int xvsnprintf(char *buf, int len, const char *fmt, va_list list)
{
    const char *cp = fmt;
    char c;
    int used = 0;
    while ((c = *cp++)) {
        if (c == '%') {
            c = *cp++;
            if (c == 'd') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 10);
            } else if (c == 'x') {
                used += xsitoa(buf + used, len - used, va_arg(list, int), 16);
            } else if (c == 's') {
                used += xsnprintf(buf + used, len - used, va_arg(list, char *));
            } else if (c == 'c') {
                buf[used] = va_arg(list, int);
                used++;
            } else if (c == '%') {
                buf[used] = c;
                used++;
            }
        } else {
            buf[used++] = c;
        }
        if (len <= used) {
            buf[len - 1] = '\0';
            return used;
        }
    }
    buf[used] = '\0';
    return used;
}

void xprintf(const char *fmt, ...)
{
    va_list list;
    va_start(list, fmt);

    char buf[256];
    xvsnprintf(buf, 256, fmt, list);
    xputs(buf);
    va_end(list);
}

#ifdef TEST

#include <string.h>

static char s_buf[256];
static int s_cnt = 0;

void buf_putchar(char c) { s_buf[s_cnt++] = c; }

void test_printf(void)
{
    // xprintf("printf") == "printf"
    xprintf("printf\r\n");

    // xprintf("d:%d x:%x", 10, 10) == "d:10 x:a"
    xprintf("d:%%\r\n");
    xprintf("d:%d\r\n", 10);
    xprintf("d:%d x:%x\r\n", 10, 10);
    xprintf("d:%d x:%x\r\n", 0xffff, 0xffff);
    xprintf("d:%d x:%x\r\n", 0x7fffffff, 0x7fffffff);
    xprintf("d:%d x:%x\r\n", 0xffffffff, 0xffffffff);
    xprintf("d:%d x:%x\r\n", -1, -1);
    xprintf("Hello, %s!\r\n", "World");
    xprintf("Hello, %c!\r\n", 'C');
}

void test_snprintf(void)
{
    char buf[256];
    int ret;

    ret = xsnprintf(buf, 255, "printf");
    assert(ret == 6);
    assert(!strcmp(buf, "printf"));

    // xprintf("d:%d x:%x", 10, 10) == "d:10 x:a"
    xsnprintf(buf, 256, "d:%d x:%x", 10, 10);
    assert(!strcmp(buf, "d:10 x:a"));

    xsnprintf(buf, 256, "d:%d x:%x", 0xffff, 0xffff);
    assert(!strcmp(buf, "d:65535 x:ffff"));

    xsnprintf(buf, 256, "d:%d x:%x", 0x7fffffff, 0x7fffffff);
    assert(!strcmp(buf, "d:2147483647 x:7fffffff"));

    xsnprintf(buf, 256, "d:%d x:%x", 0xffffffff, 0xffffffff);
    assert(!strcmp(buf, "d:-1 x:-1"));

    xsnprintf(buf, 256, "d:%d x:%x", -1, -1);
    assert(!strcmp(buf, "d:-1 x:-1"));

    xsnprintf(buf, 256, "Hello, %s!", "World");
    assert(!strcmp(buf, "Hello, World!"));

    xsnprintf(buf, 256, "Hello, %c!", 'C');
    assert(!strcmp(buf, "Hello, C!"));

    xsnprintf(buf, 10, "012345678");
    assert(!strcmp(buf, "012345678"));

    xsnprintf(buf, 10, "0123456789");
    assert(!strcmp(buf, "012345678"));
}

#endif // TEST
