/**
  ******************************************************************************
  * @file   : param_bsp.h
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : ϵͳ������������ �ײ�(ͨ��spiflash) ͷ�ļ�
  ******************************************************************************
*/
#ifndef __PARAM_BSP_H__
#define __PARAM_BSP_H__
#include "g.h"

extern int param_bsp_init(void);
extern int param_bsp_erase(unsigned int addr);
extern void param_bsp_read(unsigned char* pData, unsigned int readAddr, unsigned int  size);
extern int param_bsp_write(unsigned char* dataBuffer,unsigned int  startAddress,unsigned short ByteCount);

#endif /* __PARAM_BSP_H__ */
