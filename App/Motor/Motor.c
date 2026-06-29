#include "Motor.h"
#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "task.h"
#include "PWM.h"
#include "adc.h"


#define TASK_MOTOR_STACK_SIZE 256
#define TASK_MOTOR_PRIO       2

TaskHandle_t MotorTaskHandle = NULL;

MotorState g_motorState = MOTOR_STOP; 

/**
 * @brief  电机启动
 */
void Motor_Strat(void)
{
  
}

/**
 * @brief  电机停止
 */
void Motor_Stop(void)
{
  SetPWM(0,0);
  g_motorState = MOTOR_STOP;
}

/**
 * @brief  电机调速
 * @param  -100~+100，正数正转，负数反转，0=制动
 */
void Motor_SetSpeed(i16 speed)
{
  //限幅
  if (speed >  100) speed =  100;
  if (speed < -100) speed = -100;

  if(speed == 0){       //速度为0，制动
    SetPWM(0,0);
  }
  else if(speed > 0){   //正转
    SetPWM(speed,0);
  }
  else{               //反转
    SetPWM(0,-speed);
  }
}

/**
 * @brief 过流检测
 *        超过CURRENT_LIMIT时自动制动并置fault=1
 */
void Motor_CheckCurrent(MotorId id)
{
  uint16_t adc;
  static u8 cnt[2] = {0};
  adc = GetADCVal(id);
  
  if(adc > CURRENT_LIMIT){
    cnt[id]++;
    if(cnt[id]>5){            //连续超过阈值
      Motor_Stop();
      g_motorState = MOTOR_FAULT;
    }
  }
  else cnt[id]=0; 
}

/*********************************************************************************************************
* 函数名称：MotorTask
* 函数功能：马达任务
* 输入参数：
* 输出参数：void
* 返 回 值：void
* 创建日期：2026年05月11日
* 注    意：
*********************************************************************************************************/
static void MotorTask(void *parameters)
{
  while (1)
  {
    Motor_CheckCurrent(MOTOR_LEFT);     //实时限流检测
    Motor_CheckCurrent(MOTOR_RIGHT);
    vTaskDelay(10);
  }
}

void TaskMotorCreate(void)
{
  xTaskCreate(MotorTask,"MotorTask",TASK_MOTOR_STACK_SIZE,NULL,TASK_MOTOR_PRIO,&MotorTaskHandle);
}