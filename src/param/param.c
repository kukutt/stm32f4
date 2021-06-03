/**
  ******************************************************************************
  * @file   : param.c
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : 系统参数操作功能
  ******************************************************************************
*/
#include "param.h"

#define PARAM1_ADDR (99*BYTESPBLOCK)
#define PARAM2_ADDR (100*BYTESPBLOCK)
#define MAGICDATA 0x6B6B7474
SysParam g_Param;

int param_init(void){
    param_bsp_init();
    param_sync();
    PARAM_SYS(g_Param.rebootCount++);
    printf("reboot count = %d\r\n", g_Param.rebootCount);
    return 0;
}

uint16_t genCheck(SysParam *in){  
#define POLY        0x1021
#define INIT		0xffff
    int i,j;  
    uint16_t  crc = INIT;
    uint32_t len = sizeof(SysParam);
    uint16_t temp = in->checkSum;
    in->checkSum = 0;
    uint8_t *pSource = (uint8_t *)in;
    for (j=0; j < len; j++) {  
        crc = crc ^ (*pSource++ << 8);
        for (i = 0; i < 8; i++) {  
            if (crc & 0x8000) 
                crc = (crc << 1) ^ POLY;
            else
                crc <<= 1;
        } 
        crc &= 0xFFFF;
    }
    in->checkSum = temp;
    return crc;
}

int param_sync(void){
    uint32_t tmpMagic1 = 0;
    uint32_t tmpMagic2 = 0;
    char writeflg = 1;
    if ((g_Param.magic != MAGICDATA)){
        do{
            writeflg = 0;
            param_bsp_read((unsigned char *)&g_Param, PARAM1_ADDR, sizeof(SysParam));
            if ((g_Param.magic == MAGICDATA) && (g_Param.checkSum == genCheck(&g_Param)))break;
            param_bsp_read((unsigned char *)&g_Param, PARAM2_ADDR, sizeof(SysParam));
            if ((g_Param.magic == MAGICDATA) && (g_Param.checkSum == genCheck(&g_Param)))break;
            memset((char *)&g_Param, 0, sizeof(SysParam));
            g_Param.magic = MAGICDATA;
            writeflg = 1;
        }while(0);
    }
    
    if (writeflg){
        param_bsp_read((unsigned char *)&tmpMagic1, PARAM1_ADDR, sizeof(tmpMagic1));
        param_bsp_read((unsigned char *)&tmpMagic2, PARAM2_ADDR, sizeof(tmpMagic2));
        writeflg = 1;
        
        if ((tmpMagic1 == MAGICDATA) && (tmpMagic2 != MAGICDATA)){
            writeflg = 2;
        }
        
        g_Param.checkSum = genCheck(&g_Param);
        if (writeflg == 1){
            param_bsp_erase(PARAM1_ADDR);
            param_bsp_write((unsigned char *)&g_Param, PARAM1_ADDR, sizeof(g_Param));
            param_bsp_erase(PARAM2_ADDR);
        }else{
            param_bsp_erase(PARAM2_ADDR);
            param_bsp_write((unsigned char *)&g_Param, PARAM2_ADDR, sizeof(g_Param));
            param_bsp_erase(PARAM1_ADDR);
        }
        g_Param.checkSum = 0;
    }

#if 0
    char showparam[16];
    memset(showparam, 0, sizeof(showparam));
    param_bsp_read((unsigned char *)showparam, PARAM1_ADDR, sizeof(showparam));
    printnameandhex("PARAM1_ADDR", showparam, sizeof(showparam));
    
    memset(showparam, 0, sizeof(showparam));
    param_bsp_read((unsigned char *)showparam, PARAM2_ADDR, sizeof(showparam));
    printnameandhex("PARAM2_ADDR", showparam, sizeof(showparam));
#endif
    return 0;
}
