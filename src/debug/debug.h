/**
  ******************************************************************************
  * @file   : debug.h
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 调试功能 头文件
  ******************************************************************************
*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "g.h"

/* debug 测试时间的接口 */
#define DEBUG_PORT          GPIOB
#define DEBUG_IO		    GPIO_PIN_3
#define DEBUGIO_RESET()     HAL_GPIO_WritePin(DEBUG_PORT, DEBUG_IO, GPIO_PIN_RESET)
#define DEBUGIO_SET()       HAL_GPIO_WritePin(DEBUG_PORT, DEBUG_IO, GPIO_PIN_SET)
#define DEBUGIO_TOGGLE()    HAL_GPIO_TogglePin(DEBUG_PORT, DEBUG_IO)

int debug_init(void);
int debug_loop(void);

#endif /* __DEBUG_H__ */
