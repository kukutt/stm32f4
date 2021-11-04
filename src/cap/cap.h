/**
  ******************************************************************************
  * @file   : cap.h
  * @author : panyao
  * @version:
  * @date   : 2021,11,3
  * @brief  : adc���ݴ���ģ�� ͷ�ļ�
  ******************************************************************************
  */
#ifndef __CAP_H__
#define __CAP_H__
#include "g.h"

/* ������ 
Ƶ��λ�� = (4096*����Ƶ��*��Ƶ)/ϵͳƵ�� �ж��Ƿ�����
tim8�Ĳ���Ƶ��168M
���ۣ�
����Ƶ��					 ������ֵ(TIM8_Period/TIM8_Prescaler)
85333.33333333333 1875 			5/375
91428.57142857143 1750 			5/350
98461.53846153847 1625 			5/325
100000            1600 			10/168
ʵ��Ƶ��=168M/TIM8_Period/TIM8_Prescaler
*/
#define TIM8_Prescaler 168
#define TIM8_Period 100

void MX_ADC2_Init(void);

int cap_init(void);

int dsm_if_start1(void);
int dsm_if_start2(void);
int dsm_if_stop(void);
int dsm_if_wait(void);
int dsm_print(const char *p, int len);
int dsm_init(void);
u16_t *dsm_bufget(void);
s32_t dsm_buflenget(void);
s32_t cap_data(u8_t *data);

#endif /*__CAP_H__*/
