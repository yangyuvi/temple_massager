
#include "ProcKeyOne.h"
#include "Power.h"
#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "task.h"


#define TASK_KEY_STACK_SIZE 256       //栈大小
#define TASK_KEY_PRIO       1         //优先级

static TaskHandle_t KeyTaskHandle = NULL;
static PwrState_t pwrState = PWR_ON;

void OnKey1Event(KeyEvent event)
{ 
  switch (event)
  {
  case KEY_EVENT_UP:
  break;

  case KEY_EVENT_LONG:
    switch (pwrState)
    {
    case PWR_OFF: GPIO_SetBits(GPIOB,GPIO_Pin_1); pwrState=PWR_ON; break;  //开机
    case PWR_ON:  PowerShutdown(); pwrState=PWR_OFF; break;                //关机
    default: break;
    }
  break;

  default: break;
  }
}


/*********************************************************************************************************
* 函数名称：KeyTask
* 函数功能：按键扫描任务
* 输入参数：void
* 输出参数：u8 Mode
* 返 回 值：void
* 创建日期：2026年05月14日
* 注    意：
*********************************************************************************************************/
static void KeyTask(void *parameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1)
  {
    ScanKeyOne(OnKey1Event);
    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));       //每10ms执行一次
  }
}

void TaskKeyCreate(void)
{
  xTaskCreate(KeyTask, "KeyTask", TASK_KEY_STACK_SIZE, NULL, TASK_KEY_PRIO, &KeyTaskHandle);
}

