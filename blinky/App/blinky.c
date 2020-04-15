#include "main.h"
#include "cmsis_os.h"

extern osTimerId_t RtTimerHandle;

static void StartApp(void);
static void StartTest(void);


void StartAppTask(void *argument)
{
#if TEST
    StartTest();
#else
    StartApp();
#endif // TEST
}

static void StartApp(void)
{
  osTimerStart(RtTimerHandle, 10);
  for(;;)
  {
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
    osDelay(1000);
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
    osDelay(1000);
  }
}

#if TEST
static void StartTest(void)
{
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET);
  for(;;)
  {
  }
}
#endif // TEST

/* RtCallback function */
void RtCallback(void *argument)
{
  /* USER CODE BEGIN RtCallback */
  
  /* USER CODE END RtCallback */
}

