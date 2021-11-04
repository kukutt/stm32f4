/**
  ******************************************************************************
  * @file   : gen.c
  * @author : panyao
  * @version:
  * @date   : 2021,11,03
  * @brief  : dac数据处理模块
  ******************************************************************************
  */
#include "gen.h"

#define DACx_CHANNEL1_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define DMAx_CLK_ENABLE()               __HAL_RCC_DMA1_CLK_ENABLE()  

#define DACx_FORCE_RESET()              __HAL_RCC_DAC_FORCE_RESET()
#define DACx_RELEASE_RESET()            __HAL_RCC_DAC_RELEASE_RESET()
     
/* Definition for DACx Channel1 Pin */
#define DACx_CHANNEL1_PIN                GPIO_PIN_4
#define DACx_CHANNEL1_GPIO_PORT          GPIOA 

/* Definition for DACx's Channel1 */
#define DACx_CHANNEL1                    DAC_CHANNEL_1

/* Definition for DACx's DMA Channel1 */
#define DACx_DMA_CHANNEL1                DMA_CHANNEL_7
#define DACx_DMA_STREAM1                 DMA1_Stream5 

/* Definition for DACx's NVIC */
#define DACx_DMA_IRQn1                   DMA1_Stream5_IRQn
#define DACx_DMA_IRQHandler1             DMA1_Stream5_IRQHandler

DAC_HandleTypeDef    DacHandle;
static DAC_ChannelConfTypeDef sConfig;
#define TIM6_PERIOD 84
#define TIM6_PRESCALER 2
uint8_t aEscalator8bit[1000];
uint8_t aEscalator8bit_tmp[1000];

void DACx_DMA_IRQHandler1(void)
{
  HAL_DMA_IRQHandler(DacHandle.DMA_Handle1);
}

void HAL_DAC_MspInit(DAC_HandleTypeDef* hdac){
  GPIO_InitTypeDef          GPIO_InitStruct;
  static DMA_HandleTypeDef  hdma_dac1;
    
  /*##-1- Enable peripherals and GPIO Clocks #################################*/
  /* DAC Periph clock enable */
  __HAL_RCC_DAC_CLK_ENABLE();
  /* Enable GPIO clock ****************************************/
  DACx_CHANNEL1_GPIO_CLK_ENABLE();
 /* DMA1 clock enable */
  DMAx_CLK_ENABLE();
  
  /*##-2- Configure peripheral GPIO ##########################################*/ 
  /* DAC Channel1 GPIO pin configuration */
  GPIO_InitStruct.Pin = DACx_CHANNEL1_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(DACx_CHANNEL1_GPIO_PORT, &GPIO_InitStruct);
  
  /*##-3- Configure the DMA streams ##########################################*/
  /* Set the parameters to be configured for Channel1*/
  hdma_dac1.Instance = DACx_DMA_STREAM1;
  
  hdma_dac1.Init.Channel  = DACx_DMA_CHANNEL1;
  hdma_dac1.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_dac1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_dac1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_dac1.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
  hdma_dac1.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
  hdma_dac1.Init.Mode = DMA_CIRCULAR;
  hdma_dac1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_dac1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;         
  hdma_dac1.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_HALFFULL;
  hdma_dac1.Init.MemBurst = DMA_MBURST_SINGLE;
  hdma_dac1.Init.PeriphBurst = DMA_PBURST_SINGLE; 

  HAL_DMA_Init(&hdma_dac1);
  
  /* Associate the initialized DMA handle to the the DAC handle */
  __HAL_LINKDMA(hdac, DMA_Handle1, hdma_dac1);
  
  /*##-4- Configure the NVIC for DMA #########################################*/
   /* Enable the DMA1 Stream5 IRQ Channel */
  HAL_NVIC_SetPriority(DACx_DMA_IRQn1, 2, 0);
  HAL_NVIC_EnableIRQ(DACx_DMA_IRQn1);
}

void HAL_DAC_MspDeInit(DAC_HandleTypeDef* hdac){
  static DMA_HandleTypeDef  hdma_dac1;
  
  /*##-1- Reset peripherals ##################################################*/
  DACx_FORCE_RESET();
  DACx_RELEASE_RESET();
  
  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the DAC Channel1 GPIO pin */
  HAL_GPIO_DeInit(DACx_CHANNEL1_GPIO_PORT, DACx_CHANNEL1_PIN);
  
  /*##-3- Disable the DMA Streams ############################################*/
  /* De-Initialize the DMA Stream associate to Channel1 */
  hdma_dac1.Instance = DACx_DMA_STREAM1;
  HAL_DMA_DeInit(&hdma_dac1);
    
  /*##-4- Disable the NVIC for DMA ###########################################*/
  HAL_NVIC_DisableIRQ(DACx_DMA_IRQn1);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim){
    __HAL_RCC_TIM6_CLK_ENABLE();
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim){
    __HAL_RCC_TIM6_FORCE_RESET();
    __HAL_RCC_TIM6_RELEASE_RESET();
}


static TIM_HandleTypeDef  htim6;
void TIM6_Config(void){
  TIM_MasterConfigTypeDef sMasterConfig;
  
  /*##-1- Configure the TIM peripheral #######################################*/
  /* Time base configuration */
  htim6.Instance = TIM6;
  htim6.Init.Period = TIM6_PERIOD-1;          
  htim6.Init.Prescaler = TIM6_PRESCALER-1;       
  htim6.Init.ClockDivision = 0;    
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP; 
  HAL_TIM_Base_Init(&htim6);

  /* TIM6 TRGO selection */
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
  
  /*##-2- Enable TIM peripheral counter ######################################*/
  HAL_TIM_Base_Start(&htim6);
}


void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac){
    cap_if_start2();
}

extern s32_t gen_const_wave(u8_t *p, u32_t len, u32_t a, u32_t offset);
void DAC_Ch1_EscalatorConfig(void)
{
  /*##-1- Initialize the DAC peripheral ######################################*/
  if(HAL_DAC_Init(&DacHandle) != HAL_OK){
      while(1);
  }
  
  /*##-1- DAC channel1 Configuration #########################################*/
  sConfig.DAC_Trigger = DAC_TRIGGER_T6_TRGO;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;  
  
  if(HAL_DAC_ConfigChannel(&DacHandle, &sConfig, DACx_CHANNEL1) != HAL_OK){
      while(1);
  }
  
  /*##-2- Enable DAC Channel1 and associated DMA #############################*/
  if(HAL_DAC_Start_DMA(&DacHandle, DACx_CHANNEL1, (uint32_t*)aEscalator8bit, sizeof(aEscalator8bit), DAC_ALIGN_8B_R) != HAL_OK){
      while(1);
  }
}

int gen_init(void){
    DacHandle.Instance = DAC;
    TIM6_Config();
    DAC_Ch1_EscalatorConfig();
    return 0;
}

s32_t gen_data(u8_t *data){
    s8_t buf[100];
    s8_t *p;
    s32_t i;
    u32_t cmd = strtoul((char *)data, &p, 10);
    printf("cmd = %d\r\n", cmd);
    if (cmd == 0){
        u32_t addr = strtoul((char *)p, &p, 10);
        u32_t len = strtoul((char *)p, &p, 10);
        printf("[1]addr = %d, len=%d\r\n", addr, len);
        while(p[0] == 0x20)p++;
        HexStrToByte(p, buf, len*2);
        for (i = 0; i<len; i++){
            aEscalator8bit_tmp[addr+i] = buf[i];
        }
    }else if (cmd == 1){
        for (i = 0; i < sizeof(aEscalator8bit_tmp); i++){
            printf("%d\r\n", aEscalator8bit_tmp[i]);
        }
    }else if (cmd == 2){
        u32_t arr = strtoul((char *)p, &p, 10);
        u32_t psc = strtoul((char *)p, &p, 10);
        printf("old %d %d\r\n", htim6.Instance->ARR, htim6.Instance->PSC);
        htim6.Instance->ARR = arr;
        htim6.Instance->PSC = psc;
        printf("new %d %d\r\n", htim6.Instance->ARR, htim6.Instance->PSC);
    }else if (cmd == 100){
        printf("update\r\n");
        for (i = 0; i < sizeof(aEscalator8bit_tmp); i++){
            aEscalator8bit[i] = aEscalator8bit_tmp[i];
        }
    }else if (cmd == 101){
        printf("len = %d\r\n", sizeof(aEscalator8bit));
    }else{
        printf("cmd error\r\n");
    }
    
    return 0;
}
