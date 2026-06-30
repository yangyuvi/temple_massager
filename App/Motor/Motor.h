/**
 * @file    Motor.h
 * @brief   
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "DataType.h"


#define MOTOR_STALL_ADC_THRESHOLD           800         //失速阈值 
#define MOTOR_INRUSH_TIME_MS                80          //启动保护时间
#define MOTOR_STALL_TIME_MS                 50          //失速确认时间
#define MOTOR_STALL_COUNT                   5


typedef enum{
    MOTOR_STOP,               //停止
    MOTOR_STARTING,
    MOTOR_RUNNING,            
    MOTOR_FAULT               //过流或堵转
}MotorState;

typedef enum
{
    MOTOR_LEFT = 0,
    MOTOR_RIGHT,
    MOTOR_NUM
}MotorId;

extern MotorState g_motorState;

void  Motor_Start(void);
void  Motor_Stop(void);
void  Motor_SetSpeed(i16 speed);   //设置电机的速度
void  Motor_CheckCurrent(MotorId id);
void  TaskMotorCreate(void);

#endif
