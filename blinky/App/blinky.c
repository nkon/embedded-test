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
#else
static void StartApp(void);
#endif

void StartAppTask(void *argument)
{
#if TEST
    StartTest();
#elif HWTEST
    StartHwTest()
#else
    StartApp();
#endif // TEST
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
    ret = osTimerStart(RtTimerHandle, 100);
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
    if (i % 100) {
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
    HAL_PWR_EnterSLEEPMode(0, PWR_SLEEPENTRY_WFI);
}

#ifdef TEST

#include <string.h>

static char s_buf[256];
static char s_cnt = 0;

void buf_putchar(char c) { s_buf[s_cnt++] = c; }

static void StartTest(void)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    //    HAL_UART_Transmit(&huart2, "ABORT!", 6, 1);
    //    assert(0);

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

    // After all test pass, GREEN LED(LD2) lights.
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    for (;;) {
    }
}
#endif // TEST

#ifdef HWTEST

static void StartHwTest(void) {}

#endif // TEST
