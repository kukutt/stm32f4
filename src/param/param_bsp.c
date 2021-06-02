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


int param_bsp_init(void){
    return 0;
}

int param_bsp_erase(unsigned int addr){
    return 0;
}
void param_bsp_read(unsigned char* pData, unsigned int readAddr, unsigned int  size){
    ;
}
int param_bsp_write(unsigned char* dataBuffer,unsigned int  startAddress,unsigned short ByteCount){
    return 0;
}
