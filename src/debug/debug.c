/**
  ******************************************************************************
  * @file   : debug.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 调试功能
  ******************************************************************************
*/
#include "g.h"

UART_HandleTypeDef UartHandle;
#define USARTx                           USART1
#define USARTx_CLK_ENABLE()              __HAL_RCC_USART1_CLK_ENABLE();
#define USARTx_RX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE()
#define USARTx_TX_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOA_CLK_ENABLE() 

#define USARTx_FORCE_RESET()             __HAL_RCC_USART1_FORCE_RESET()
#define USARTx_RELEASE_RESET()           __HAL_RCC_USART1_RELEASE_RESET()
    
/* Definition for USARTx Pins */
#define USARTx_TX_PIN                    GPIO_PIN_9
#define USARTx_TX_GPIO_PORT              GPIOA  
#define USARTx_TX_AF                     GPIO_AF7_USART1
#define USARTx_RX_PIN                    GPIO_PIN_10
#define USARTx_RX_GPIO_PORT              GPIOA 
#define USARTx_RX_AF                     GPIO_AF7_USART1

s32_t debug_init(void){
    GPIO_InitTypeDef  GPIO_InitStruct;

    /*##-1- Enable peripherals and GPIO Clocks #################################*/
    /* Enable GPIO TX/RX clock */
    USARTx_TX_GPIO_CLK_ENABLE();
    USARTx_RX_GPIO_CLK_ENABLE();
    /* Enable USART2 clock */
    USARTx_CLK_ENABLE();

    /*##-2- Configure peripheral GPIO ##########################################*/
    /* UART TX GPIO pin configuration  */
    GPIO_InitStruct.Pin       = USARTx_TX_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = USARTx_TX_AF;

    HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_InitStruct);

    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin = USARTx_RX_PIN;
    GPIO_InitStruct.Alternate = USARTx_RX_AF;

    HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_InitStruct);
  
    UartHandle.Instance          = USARTx;
    UartHandle.Init.BaudRate     = 115200;
    UartHandle.Init.WordLength   = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits     = UART_STOPBITS_1;
    UartHandle.Init.Parity       = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode         = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    
    if(HAL_UART_Init(&UartHandle) != HAL_OK){
        while(1);
    }
    return 0;
}

int fputc(int ch, FILE *f){
    s8_t tmp = ch;
    HAL_UART_Transmit(&UartHandle, (uint8_t*)&tmp, 1, 500);
    return ch;
}

#ifdef BOOTFLG
#define MAXUARTLEN 1200
#else
#define MAXUARTLEN 128
#endif
uint8_t saveuart[MAXUARTLEN+4];
uint16_t saveuarti=0;
int debug_loop(void){
    char g_uartData;
    if(HAL_UART_Receive(&UartHandle, (uint8_t *)&g_uartData, 1, 500) == HAL_OK){
        if ((g_uartData == '\r') || (g_uartData == '\n') || (g_uartData == ';') || (saveuarti > MAXUARTLEN)){
            saveuart[saveuarti] = 0;
            printf("\r\n");
            
            if (0 == memcmp("test", saveuart, 4)){
#ifndef BOOTFLG
                printf("app_[%s]\r\n", APPVER);
#else
                printf("boot_[%s]\r\n", BOOTVER);
#endif
            }else if (0 == memcmp("reboot", saveuart, 7)){
                reboot();
#ifdef BOOTFLG
            }else if (0 == memcmp("iap_run", saveuart, 7)){
                IAP_UpdateCheck(1);
#endif
            }else if (0 == memcmp("iap_init", saveuart, 8)){
                if (HAL_OK == HAL_FLASH_Unlock()){
                    printf("req:init ok\r\n");
                }else{
                    printf("req:init error\r\n");
                }
            }else if (0 == memcmp("iap_read", saveuart, 8)){
                s8_t *p;
                u32_t addr = strtoul((char *)&saveuart[8], &p, 16);
                u32_t len = strtoul(p, &p, 10);
                printf("%08x %d\r\n", addr, len);
                printnameandhex("read", (char *)addr, len);
            }else if (0 == memcmp("iap_erase", saveuart, 9)){
                u32_t addr = strtoul((char *)&saveuart[9], NULL, 16);
                s32_t sector = -1;
                FLASH_EraseInitTypeDef EraseInitStruct;
                uint32_t SectorError = 0;
                
                if ((0x8000000 <= addr) && (0x8010000 > addr)){
                    sector = (addr-0x8000000) >> 14;
                }else if((0x8010000 <= addr) && (0x8020000 > addr)){
                    sector = 4;
                }else if((0x8020000 <= addr) && (0x8100000 > addr)){
                    sector = ((addr-0x8020000) >> 17)+5;
                }else{
                    sector = -1;
                }
                
                if (sector >= 0){
                    EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
                    EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
                    EraseInitStruct.Sector = sector;
                    EraseInitStruct.NbSectors = 1;
                    if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) == HAL_OK){
                        printf("req:erase ok [%08x][%d]\r\n", addr, sector);
                    }else{
                        printf("req:erase error1 [%08x][%d]\r\n", addr, sector);
                    }
                }else{
                    printf("req:erase error2 [%08x][%d]\r\n", addr, sector);
                }
            }else if (0 == memcmp("iap_write", saveuart, 9)){
                s8_t buf[520];
                s8_t *p;
                s32_t i;
                s32_t errorflg = 0;
                u32_t addr = strtoul((char *)&saveuart[9], &p, 16);
                p+=1;
                u32_t len = strlen(p);
                u32_t tmp;
                HexStrToByte(p, buf, len);
                len = len / 2;
                for (i = 0; i<len; i+=4){
                    tmp = 0;
                    ((s8_t *)&tmp)[0] = buf[i+0];
                    ((s8_t *)&tmp)[1] = buf[i+1];
                    ((s8_t *)&tmp)[2] = buf[i+2];
                    ((s8_t *)&tmp)[3] = buf[i+3];
                    if (HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addr+i, tmp)){
                        printf("req:write error [%08x]\r\n", addr);
                        errorflg = 1;
                    }
                }
                if (0 == errorflg){
                    printf("req:write ok [%08x][%d]\r\n", addr, len);
                }
#ifndef BOOTFLG
            }else if (0 == memcmp("time", saveuart, 4)){
                printf("now=%d\r\n", time_delay_ms(0));
#endif
            }else{
                printf("error\r\n");
                printnameandhex("hex", (s8_t *)saveuart, saveuarti);
            }
            saveuarti=0;
        }else{
            saveuart[saveuarti] = g_uartData;
            saveuarti++;
        }
        
    }
    return 0;
}
