/**
  ******************************************************************************
  * @file   : param_bsp.h
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : 系统参数操作功能 底层(通过spiflash) 头文件
  ******************************************************************************
*/
#ifndef __PARAM_BSP_H__
#define __PARAM_BSP_H__
#include "g.h"

//W25X系列/Q系列芯片列表	   
//W25Q80  ID  0XEF13
//W25Q16  ID  0XEF14
//W25Q32  ID  0XEF15
//W25Q64  ID  0XEF16	
//W25Q128 ID  0XEF17	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16
#define W25Q128	0XEF17

////////////////////////////////////////////////////////////////////////////////// 
//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 


extern int param_bsp_init(void);
extern int param_bsp_erase(unsigned int addr);
extern void param_bsp_read(unsigned char* pData, unsigned int readAddr, unsigned int  size);
extern int param_bsp_write(unsigned char* dataBuffer,unsigned int  startAddress,unsigned short ByteCount);

#endif /* __PARAM_BSP_H__ */
