/**
  ******************************************************************************
  * @file   : task.h
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 任务调度功能 头文件
  ******************************************************************************
*/
#ifndef __TASK_H__
#define __TASK_H__

#include "g.h"

extern s32_t task_init(void);
extern s32_t task_loop(void);
extern s32_t task_add(void (*fn)(void), u32_t timeout);

#endif /* __TASK_H__ */
