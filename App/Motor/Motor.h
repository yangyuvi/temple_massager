/**
 * @file    Motor.h
 * @brief   
 */

#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "DataType.h"

//电流保护阈值 ma
#define CURRENT_LIMIT          800


typedef enum{
    MOTOR_FORWARD,            //正转 IN1=PWM IN2=0
    MOTOR_REVERSE,            //反转 IN1=0 IN2=PWM
    MOTOR_STOP,               //停止 IN1=1 IN2=1
    MOTOR_FAULT               //过流或堵转
}MotorState;

typedef enum
{
    MOTOR_LEFT = 0,
    MOTOR_RIGHT
}MotorId;

extern MotorState g_motorState;

void  Motor_Start(void);
void  Motor_Stop(void);
void  Motor_SetSpeed(i16 speed);   //设置电机的速度
void  Motor_CheckCurrent(MotorId id);
void  TaskMotorCreate(void);

#endif
