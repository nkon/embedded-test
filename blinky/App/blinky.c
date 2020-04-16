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
#else
    StartApp();
#endif // TEST
}

void xputchar(char c) { HAL_UART_Transmit(&huart2, (uint8_t *)&c, 1, 1); }

static void StartApp(void)
{
    int i = 0;
    osTimerStart(RtTimerHandle, 10);
    for (;;) {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        osDelay(100);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        osDelay(100);
        xprintf("printf\r\n");
        xprintf("%d ", 1);
        xprintf("%d ", i++);
        xprintf("%d ", -65535);
    }
}

#ifdef TEST
static void StartTest(void)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
    for (;;) {
    }
}
#endif // TEST

/* RtCallback function */
void RtCallback(void *argument)
{
    /* USER CODE BEGIN RtCallback */

    /* USER CODE END RtCallback */
}
