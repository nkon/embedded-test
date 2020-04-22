#include <stdbool.h>

#include "assert.h"
#include "cmsis_os.h"
#include "main.h"
#include "printf.h"

extern osTimerId_t RtTimerHandle;
extern UART_HandleTypeDef huart2;

// #define TEST

#ifdef TEST
static void StartTest(void);
#elif HWTEST
static void StartHwTest(void);
#else
static void StartApp(void);
#endif

void StartDefaultTask(void *argument)
{
#if TEST
    StartTest();
#elif HWTEST
    StartHwTest();
#else
    StartApp();
#endif // TEST
    assert(0);
}

void uart_putchar(char c) { HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 1); }

static void StartApp(void)
{
    int i = 0;
    osStatus_t ret;
    xputchar = uart_putchar;
    xprintf("StartApp().\r\n");
    if (!RtTimerHandle) {
        xprintf("timer creation failuer.\r\n");
        assert(0);
    }
    ret = osTimerStart(RtTimerHandle, 1);
    if (ret != osOK) {
        xprintf("timer start failuer.\r\n");
        assert(0);
    }
    for (;;) {
        osDelay(100);
        i++;
        xprintf("d:%d x:%x\r\n", i, i, i);
    }
}

/* RtCallback function */
void RtCallback(void *argument)
{
    /* USER CODE BEGIN RtCallback */
    static uint32_t i;
    static bool led;
    if (i % 200 == 0) {
        if (led) {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            led = false;
        } else {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            led = true;
        }
    }
    i++;
    if (i >= 2147483600) {
        i = 0;
    }
    /* USER CODE END RtCallback */
}

void vApplicationIdleHook(void)
{
    //    HAL_UART_Transmit(&huart2, "S", 1, 1);
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}

#ifdef TEST

#include <string.h>

static char s_buf[256];
static char s_cnt = 0;

void buf_putchar(char c) { s_buf[s_cnt++] = c; }

static void test_printf(void)
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

static void StartTest(void)
{
    xputchar = uart_putchar;

    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    // HAL_UART_Transmit(&huart2, "ABORT!", 6, 1);
    // assert(0);

    test_printf();

    // After all test pass, GREEN LED(LD2) lights.
    xputchar = uart_putchar;
    xprintf("ALL TEST CASES: PASS\r\n");
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    for (;;) {
    }
}
#endif // TEST

#ifdef HWTEST

static bool button_pushed(void)
{
    // GPIO_PIN_SET => button is not pushed.
    if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET) {
        return false;
    }
    osDelay(150);
    if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_SET) {
        return false;
    }
    return true;
}

static void StartHwTest(void)
{

    xputchar = uart_putchar;
    xprintf("push button to start HW_TEST\r\n");

    while (!button_pushed())
        ;
    xprintf("LED turn on\r\n");
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);

    while (!button_pushed())
        ;
    xprintf("LED turn off\r\n");
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    while (!button_pushed())
        ;

    // After all test pass, GREEN LED(LD2) lights.
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    xprintf("HW_TEST completed!\r\n");
    for (;;) {
    }
}

#endif // HWTEST
