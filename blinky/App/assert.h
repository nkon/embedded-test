#ifndef ASSERT_H
#define ASSERT_H

#ifdef NDEBUG
#define assert(x) ((void)0)
#else
#define assert(x)                                                              \
    do {                                                                       \
        if (!(x)) {                                                            \
            HAL_UART_Transmit(&huart2, "ABORT!", 6, 1);                        \
            while (1)                                                          \
                ;                                                              \
        }                                                                      \
    } while (0);
#endif

#endif // ASSERT_H
