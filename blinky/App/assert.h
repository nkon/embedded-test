#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x)                                                              \
    do {                                                                       \
        if (!(x)) {                                                            \
            xprintf(" *** Assertion Fail *** File:%s Line:%d", __FILE__,       \
                    __LINE__);                                                 \
            while (1)                                                          \
                ;                                                              \
        }                                                                      \
    } while (0);
#endif

#endif // ASSERT_H
