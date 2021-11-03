/**
  ******************************************************************************
  * @file   : cap.c
  * @author : panyao
  * @version:
  * @date   : 2021,11,3
  * @brief  : adc数据处理模块
  ******************************************************************************
  */
#include "cap.h"

#define OFFSET 802
#define TEST_LENGTH_SAMPLES (8000+OFFSET)
u16_t ADC_Value[TEST_LENGTH_SAMPLES];

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;
volatile int dma_IT_flag=1;

/* adc */
#define wl_Pin GPIO_PIN_2
#define ib_Pin GPIO_PIN_3
#define ADC_GPIO_Port GPIOC

#define SLCAP_Pin GPIO_PIN_7
#define SLCAP_GPIO_Port GPIOA
#define SLCAP_ADC_CH ADC_CHANNEL_7
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc2;
TIM_HandleTypeDef htim8;

void DMA2_Stream2_IRQHandler(void){
    HAL_DMA_IRQHandler(&hdma_adc2);
}


void ADC_IRQHandler(void){
    HAL_ADC_IRQHandler(&hadc2);
}

/* TIM8 init function */
int fs;
void MX_TIM8_Init(void)
{
    TIM_MasterConfigTypeDef sMasterConfig;
    __HAL_RCC_TIM8_CLK_ENABLE();

    /* Time Base configuration */
    htim8.Instance = TIM8;

    htim8.Init.Period = TIM8_Period - 1;          
    htim8.Init.Prescaler = TIM8_Prescaler - 1;     
    fs = SystemCoreClock / (htim8.Init.Period + 1) / (htim8.Init.Prescaler + 1);
    htim8.Init.ClockDivision = 0;    
    htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim8.Init.RepetitionCounter = 0;
    htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  
    if(HAL_TIM_Base_Init(&htim8) != HAL_OK){
        while(1);
    }
  
    /* TIM8 TRGO selection */
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK){
        while(1);
    }
}

void MX_DMA_ADC_Init(void) {
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA2_Stream2_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
}

void MX_ADC2_Init(void)
{
  ADC_ChannelConfTypeDef sConfig = {0};

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;//ENABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T8_TRGO;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
      while(1);
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
  */
  sConfig.Channel = SLCAP_ADC_CH; /* 5:MZM(开发板采集PA5) 6:DSB */
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
      while(1);
  }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspInit 0 */

  /* USER CODE END ADC2_MspInit 0 */
    /* ADC2 clock enable */
    __HAL_RCC_ADC2_CLK_ENABLE();
  
    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**ADC2 GPIO Configuration    
    PA5     ------> ADC2_IN5
    PA6     ------> ADC2_IN6 
    */
    GPIO_InitStruct.Pin = SLCAP_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(SLCAP_GPIO_Port, &GPIO_InitStruct);

    /* ADC2 DMA Init */
    /* ADC2 Init */
    hdma_adc2.Instance = DMA2_Stream2;
    hdma_adc2.Init.Channel = DMA_CHANNEL_1;
    hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc2.Init.Mode = DMA_NORMAL;
    hdma_adc2.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
    {
      while(1);
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc2);

    /* ADC2 interrupt Init */
    HAL_NVIC_SetPriority(ADC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC_IRQn);
  /* USER CODE BEGIN ADC2_MspInit 1 */

  /* USER CODE END ADC2_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC2)
  {
  /* USER CODE BEGIN ADC2_MspDeInit 0 */

  /* USER CODE END ADC2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC2_CLK_DISABLE();
  
    /**ADC2 GPIO Configuration    
    PA5     ------> ADC2_IN5
    PA6     ------> ADC2_IN6 
    */
    HAL_GPIO_DeInit(SLCAP_GPIO_Port, SLCAP_Pin);

    /* ADC2 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);

    /* ADC2 interrupt Deinit */
  /* USER CODE BEGIN ADC2:ADC_IRQn disable */
    /**
    * Uncomment the line below to disable the "ADC_IRQn" interrupt
    * Be aware, disabling shared interrupt may affect other IPs
    */
    /* HAL_NVIC_DisableIRQ(ADC_IRQn); */
  /* USER CODE END ADC2:ADC_IRQn disable */

  /* USER CODE BEGIN ADC2_MspDeInit 1 */

  /* USER CODE END ADC2_MspDeInit 1 */
  }
} 

int cap_init(void){
    MX_DMA_ADC_Init();
    MX_ADC2_Init();
    MX_TIM8_Init();
    return 0;
}


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
