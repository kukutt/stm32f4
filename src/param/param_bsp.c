/**
  ******************************************************************************
  * @file   : param_bsp.c
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : 系统参数操作功能 底层(通过spiflash)
  ******************************************************************************
*/
#include "param_bsp.h"

u16_t W25QXX_ReadID(void);

/* Definition for SPIx clock resources */
#define SPIx                             SPI1
#define SPIx_CLK_ENABLE()                __HAL_RCC_SPI1_CLK_ENABLE()
#define SPIx_SCK_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define SPIx_MISO_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 
#define SPIx_MOSI_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE() 

#define SPIx_FORCE_RESET()               __HAL_RCC_SPI1_FORCE_RESET()
#define SPIx_RELEASE_RESET()             __HAL_RCC_SPI1_RELEASE_RESET()


#define SPI1CS_PORT         GPIOB
#define SPI1CS_IO		    GPIO_PIN_0

#define W25QXX_CS_RESET()   HAL_GPIO_WritePin(SPI1CS_PORT, SPI1CS_IO, GPIO_PIN_RESET)
#define W25QXX_CS_SET()     HAL_GPIO_WritePin(SPI1CS_PORT, SPI1CS_IO, GPIO_PIN_SET)

/* Definition for SPIx Pins */
#define SPIx_SCK_PIN                     GPIO_PIN_3
#define SPIx_SCK_GPIO_PORT               GPIOB
#define SPIx_SCK_AF                      GPIO_AF5_SPI1
#define SPIx_MISO_PIN                    GPIO_PIN_4
#define SPIx_MISO_GPIO_PORT              GPIOB
#define SPIx_MISO_AF                     GPIO_AF5_SPI1
#define SPIx_MOSI_PIN                    GPIO_PIN_5
#define SPIx_MOSI_GPIO_PORT              GPIOB
#define SPIx_MOSI_AF                     GPIO_AF5_SPI1

void W25QXX_SPI_IO_init(void){
    GPIO_InitTypeDef  GPIO_InitStruct;

    SPIx_SCK_GPIO_CLK_ENABLE();
    SPIx_MISO_GPIO_CLK_ENABLE();
    SPIx_MOSI_GPIO_CLK_ENABLE();
    SPIx_CLK_ENABLE(); 

    GPIO_InitStruct.Pin       = SPIx_SCK_PIN;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_PULLUP;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FAST;
    GPIO_InitStruct.Alternate = SPIx_SCK_AF;

    HAL_GPIO_Init(SPIx_SCK_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPIx_MISO_PIN;
    GPIO_InitStruct.Alternate = SPIx_MISO_AF;

    HAL_GPIO_Init(SPIx_MISO_GPIO_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = SPIx_MOSI_PIN;
    GPIO_InitStruct.Alternate = SPIx_MOSI_AF;

    HAL_GPIO_Init(SPIx_MOSI_GPIO_PORT, &GPIO_InitStruct);    
}

SPI_HandleTypeDef SpiHandle;
void W25QXX_SPI_init(void){
    SpiHandle.Instance               = SPIx;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;

    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    if(HAL_SPI_Init(&SpiHandle) != HAL_OK){
        while(1);
    }
}


void W25QXX_Init(void){ 
    /* DEBUG IO */
    GPIO_InitTypeDef  GPIO_InitStruct;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = SPI1CS_IO;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(SPI1CS_PORT, &GPIO_InitStruct);
	W25QXX_CS_SET();
    
    W25QXX_SPI_IO_init();
    W25QXX_SPI_init();
	W25QXX_ReadID();
}  

u8_t W25QXX_ReadSR(void){
	u8_t tbyte[2];
	u8_t rbyte[2];
	W25QXX_CS_RESET();
    tbyte[0] = W25X_ReadStatusReg;
    tbyte[1] = 0xff;
    HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)tbyte, (uint8_t *)rbyte, 2, 5000);
	W25QXX_CS_SET();
	return rbyte[1];
}

void W25QXX_Write_SR(u8_t sr){
	u8_t tbyte[2];
	W25QXX_CS_RESET();
    tbyte[0] = W25X_WriteStatusReg;
    tbyte[1] = sr;
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 2, 5000);
	W25QXX_CS_SET();
}

void W25QXX_Write_Enable(void){
	u8_t tbyte[2];
	W25QXX_CS_RESET();
    tbyte[0] = W25X_WriteEnable;
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 1, 5000);
	W25QXX_CS_SET();
}

void W25QXX_Write_Disable(void){
	u8_t tbyte[2];
	W25QXX_CS_RESET();
    tbyte[0] = W25X_WriteDisable;
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 1, 5000);
	W25QXX_CS_SET();	      
}

u16_t W25QXX_ReadID(void){
	u16_t Temp = 0;	
	u8_t tbyte[6];
	u8_t rbyte[6];
	W25QXX_CS_RESET();
    tbyte[0] = 0x90;
    tbyte[1] = 0x00;
    tbyte[2] = 0x00;
    tbyte[3] = 0x00;
    tbyte[4] = 0xff;
    tbyte[5] = 0xff;
    HAL_SPI_TransmitReceive(&SpiHandle, (uint8_t*)tbyte, (uint8_t *)rbyte, 6, 5000);
    Temp = (u16_t)(rbyte[4]<<8) | rbyte[5];
	W25QXX_CS_SET();
    //printf("SPI_ID: %04x %02x-%02x\r\n", Temp, rbyte[4], rbyte[5]);
	return Temp;
}

void W25QXX_Wait_Busy(void){   
	while((W25QXX_ReadSR()&0x01)==0x01);
}

void W25QXX_Erase_Chip(void){
	u8_t tbyte[2];
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
	W25QXX_CS_RESET();
    tbyte[0] = W25X_ChipErase;
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 1, 5000);
	W25QXX_CS_SET();
	W25QXX_Wait_Busy();
}   

void W25QXX_Erase_Sector(u32_t Dst_Addr){
	u8_t tbyte[4];
    
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    
	W25QXX_CS_RESET();
    tbyte[0] = W25X_SectorErase;
    tbyte[1] = (u8_t)((Dst_Addr)>>16);
    tbyte[2] = (u8_t)((Dst_Addr)>>8);
    tbyte[3] = (u8_t)(Dst_Addr);
    
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 4, 5000);
	W25QXX_CS_SET();
    W25QXX_Wait_Busy();
}

void W25QXX_Read(u8_t* pBuffer,u32_t ReadAddr,u16_t NumByteToRead){
	u8_t tbyte[4];
	W25QXX_CS_RESET();
    
    tbyte[0] = W25X_ReadData;
    tbyte[1] = (u8_t)((ReadAddr)>>16);
    tbyte[2] = (u8_t)((ReadAddr)>>8);
    tbyte[3] = (u8_t)ReadAddr;
    
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 4, 5000);
    HAL_SPI_Receive(&SpiHandle, (uint8_t*)pBuffer, NumByteToRead, 5000);
	W25QXX_CS_SET();
}

void W25QXX_Write_Page(u8_t* pBuffer,u32_t WriteAddr,u16_t NumByteToWrite){
	u8_t tbyte[4];
    W25QXX_Write_Enable();
	W25QXX_CS_RESET();
    
    tbyte[0] = W25X_PageProgram;
    tbyte[1] = (u8_t)((WriteAddr)>>16);
    tbyte[2] = (u8_t)((WriteAddr)>>8);
    tbyte[3] = (u8_t)WriteAddr;
    
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)tbyte, 4, 5000);
    HAL_SPI_Transmit(&SpiHandle, (uint8_t*)pBuffer, NumByteToWrite, 5000);
	W25QXX_CS_SET();
    W25QXX_Wait_Busy();
} 

int param_bsp_init(void){
    W25QXX_Init();
    return 0;
}

int param_bsp_erase(unsigned int addr){
    W25QXX_Erase_Sector(addr);
    return 0;
}
void param_bsp_read(unsigned char* pData, unsigned int readAddr, unsigned int  size){
    W25QXX_Read(pData, readAddr, size);
}
int param_bsp_write(unsigned char* dataBuffer,unsigned int  startAddress,unsigned short ByteCount){
    W25QXX_Write_Page(dataBuffer, startAddress, ByteCount);
    return 0;
}
