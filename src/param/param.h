/**
  ******************************************************************************
  * @file   : param.h
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : 系统参数操作功能 头文件
  ******************************************************************************
*/
#ifndef __PARAM_H__
#define __PARAM_H__

#include "g.h"
#include "param_bsp.h"

#define BYTESPBLOCK (4096)

#define PARAM_SYS(A)    {A;param_sync();}

typedef struct{
    uint32_t magic;
    uint16_t checkSum;
    uint16_t rebootCount;//系统重启计数，每启动一次加一即可
}SysParam;
extern SysParam g_Param;

extern int param_init(void);
extern int param_sync(void);

#endif /* __PARAM_H__ */
