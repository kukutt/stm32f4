/**
  ******************************************************************************
  * @file   : cap_if.c
  * @author : panyao
  * @version:
  * @date   : 2021,7,12
  * @brief  : adc数据处理模块
  ******************************************************************************
  */
#include "g.h"

#define OFFSET 802
#define TEST_LENGTH_SAMPLES (8000+OFFSET)
u16_t ADC_Value[TEST_LENGTH_SAMPLES];

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;
extern TIM_HandleTypeDef htim8;
volatile int dma_IT_flag=1;
extern void MX_ADC2_chanelset(int sw);

int dsm_if_start1(void){
	memset((char *)ADC_Value, 0, sizeof(ADC_Value));
	dma_IT_flag = 0;
	return 0;
}

int dsm_if_start2(void){
    if (dma_IT_flag != 0)return -1;
#ifdef TIM8_INT
    HAL_TIM_Base_Start_IT(&htim8);
#else
    HAL_TIM_Base_Start(&htim8);
#endif
    HAL_ADC_Start_DMA(&hadc2,(uint32_t *)ADC_Value,TEST_LENGTH_SAMPLES);
	return 0;
}


int dsm_if_stop(void){
    HAL_ADC_Stop_DMA(&hadc2);
#ifndef TIM8_INT
    HAL_TIM_Base_Stop(&htim8);
#endif
    return 0;
}


int dsm_if_wait(void){
    while(dma_IT_flag==0){
    }
    return 0;
}

int dsm_cb(void){
    dma_IT_flag = 1;
	return 0;
}



void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
    dsm_cb();
}

int dsm_print(const char *p, int len){
    int i = 0;
    int maxlen = TEST_LENGTH_SAMPLES;
    
    if (len != 0)maxlen = len;
    printf("%s:\r\n", p);
    for (i = OFFSET; i < maxlen; i++){
        printf("%d,%d\r\n", i, ADC_Value[i]);
    }
    printf("%s:\r\n", p);
    
    return 0;
}

u16_t *dsm_bufget(void){
    return ADC_Value;
}

s32_t dsm_buflenget(void){
    return TEST_LENGTH_SAMPLES;
}

