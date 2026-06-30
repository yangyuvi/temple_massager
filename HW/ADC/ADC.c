/**
 ******************************************************************************
 * @file           : ADC.c
 * @brief          : 
 ******************************************************************************
 * @attention
 * 
 * ADC1_CH4: 左电机IPROPI
 * ADC1_CH5: 右电机IPROPI
 * ADC1_CH8: 电池电压
 *
 *
 ******************************************************************************
 */

#include "ADC.h"
#include "stm32f10x_conf.h"
#include "FreeRTOS.h"
#include "task.h"

#define FILTER_SIZE 8
#define ADC_NUM     3


static u16 s_arrADCData[ADC_NUM];   //存放ADC转换结果数据
static u16 s_arrADCFilter[ADC_NUM];

//内部函数声明
static void ConfigADC1(void);     //配置ADC1
static void ConfigDMA1Ch1(void);  //配置DMA通道1

/**
 * @brief  配置ADC1
 * @attention ADC1_IN4-PA4 ADC1_IN5-PA5
 */
static void ConfigADC1(void)
{                          
  GPIO_InitTypeDef  GPIO_InitStructure; //GPIO_InitStructure用于存放GPIO的参数
  ADC_InitTypeDef   ADC_InitStructure;  //ADC_InitStructure用于存放ADC的参数

  //使能RCC相关时钟
  RCC_ADCCLKConfig(RCC_PCLK2_Div6); //设置ADC时钟分频，ADCCLK=PCLK2/6=12MHz
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1  , ENABLE);  //使能ADC1的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);  //使能GPIOA的时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);  //使能GPIOB的时钟
 
  //配置ADC1的GPIO
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 | GPIO_Pin_5;    //设置引脚
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN; //设置输入类型
  GPIO_Init(GPIOA, &GPIO_InitStructure);  //根据参数初始化GPIO

  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;    //设置引脚
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN; //设置输入类型
  GPIO_Init(GPIOB, &GPIO_InitStructure);  //根据参数初始化GPIO

  //配置ADC1
  ADC_InitStructure.ADC_Mode               = ADC_Mode_Independent;  //设置为独立模式
  ADC_InitStructure.ADC_ScanConvMode       = ENABLE;                //使能扫描模式
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                //连续转换模式
  ADC_InitStructure.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;  //不使用硬件触发
  ADC_InitStructure.ADC_DataAlign          = ADC_DataAlign_Right;   //设置为右对齐
  ADC_InitStructure.ADC_NbrOfChannel       = 3; //设置ADC的通道数目
  ADC_Init(ADC1, &ADC_InitStructure);

  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_71Cycles5); //设置采样时间为239.5个周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_71Cycles5); //设置采样时间为239.5个周期
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_71Cycles5); //设置采样时间为239.5个周期

  ADC_DMACmd(ADC1, ENABLE);                   //使能ADC1的DMA
  ADC_Cmd(ADC1, ENABLE);                      //使能ADC1

  ADC_ResetCalibration(ADC1);                 //启动ADC复位校准，即将RSTCAL赋值为1
  while(ADC_GetResetCalibrationStatus(ADC1)); //读取并判断RSTCAL，RSTCAL为0跳出while语句
  ADC_StartCalibration(ADC1);                 //启动ADC校准，即将CAL赋值为1
  while(ADC_GetCalibrationStatus(ADC1));      //读取并判断CAL，CAL为0跳出while语句
 
}


/**
 * @brief  配置DMA通道1
 */
static void ConfigDMA1Ch1(void)
{
  DMA_InitTypeDef DMA_InitStructure;  //DMA_InitStructure用于存放DMA的参数
  
  //使能RCC相关时钟
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);  //使能DMA1的时钟
  
  //配置DMA1_Channel1
  DMA_DeInit(DMA1_Channel1);  //将DMA1_CH1寄存器设置为默认值
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);           //设置外设地址
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)s_arrADCData;         //设置存储器地址
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;           //设置为外设到存储器模式
  DMA_InitStructure.DMA_BufferSize         = 3;                               //设置要传输的数据项数目
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       //设置外设为非递增模式
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;            //设置存储器为递增模式
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //设置外设数据长度为半字
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;     //设置存储器数据长度为半字
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;               //设置为循环模式
  DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;             //设置为中等优先级
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;                 //禁止存储器到存储器访问
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);  //根据参数初始化DMA1_Channel1
  
  DMA_Cmd(DMA1_Channel1, ENABLE); //使能DMA1_Channel1
}

/**
 * @brief  ADC初始化
 */
void InitADC(void)
{
  u8 i;

  ConfigDMA1Ch1();          //配置DMA1的通道1  
  ConfigADC1();             //配置ADC1
  
  ADC_SoftwareStartConvCmd(ADC1, ENABLE);     //使用软件触发

  //启动后先采样一次，避免从0爬升
  for(i=0;i<ADC_NUM;i++)
  {
    s_arrADCFilter[i]=s_arrADCData[i];
  }
}


/**
 * @brief  通用一阶低通滤波算法
 */
void ADCFilter(void)
{
  u8 i;

  for(i=0; i<ADC_NUM; i++){
    s_arrADCFilter[i] = (s_arrADCFilter[i]*7 + s_arrADCData[i]) / 8;
  }
}

/**
 * @brief  获取ADC值
 * @param  id
 */
u16 GetADCVal(u8 id)
{
  return s_arrADCFilter[id];
}

