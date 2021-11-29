/**
  ******************************************************************************
  * @file   : cap_if.h
  * @author : panyao
  * @version:
  * @date   : 2021,7,12
  * @brief  : adc数据处理模块 头文件
  ******************************************************************************
  */

#ifndef __CAP_IF_H__
#define __CAP_IF_H__
#include "g.h"
#include "cap.h"

/* 采样率 
频率位置 = (4096*抖动频率*分频)/系统频率 判断是否整除
tim8的采样频率168M
结论：
中心频率					 设置数值(TIM8_Period/TIM8_Prescaler)
85333.33333333333 1875 			5/375
91428.57142857143 1750 			5/350
98461.53846153847 1625 			5/325
100000            1600 			10/168
实际频率=168M/TIM8_Period/TIM8_Prescaler
*/
#define TIM8_Prescaler 210
#define TIM8_Period 100

//#define TIM8_INT

int dsm_if_start1(void);
int dsm_if_start2(void);
int dsm_if_stop(void);
int dsm_if_wait(void);
int dsm_print(const char *p, int len);
int dsm_init(void);
u16_t *dsm_bufget(void);
s32_t dsm_buflenget(void);

#endif /*__CAP_IF_H__*/
