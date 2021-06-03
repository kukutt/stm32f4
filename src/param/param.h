/**
  ******************************************************************************
  * @file   : param.h
  * @author : panyao
  * @version:
  * @date   : 2021,6,2
  * @brief  : ϵͳ������������ ͷ�ļ�
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
    uint16_t rebootCount;//ϵͳ����������ÿ����һ�μ�һ����
}SysParam;
extern SysParam g_Param;

extern int param_init(void);
extern int param_sync(void);

#endif /* __PARAM_H__ */
