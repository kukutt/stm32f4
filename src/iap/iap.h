/**
  ******************************************************************************
  * @file   : iap.h
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : iap升级功能 头文件 
  ******************************************************************************
*/

#ifndef __IAP_H__
#define __IAP_H__

#include "g.h"



#define SRAM_START                  ((uint32_t)0x20000000)
#define SRAM_END                    ((uint32_t)0x20020000)

#define BOOT_PARA_ADDRESS           (0x0800c000)


#define APP_FLAG                    (0x1122335a)
#define APP_ADDRESS                 (0x08020000)

void IAP_UpdateCheck(u32_t checkflg);

#endif /* __IAP_H__ */
