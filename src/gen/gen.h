/**
  ******************************************************************************
  * @file   : gen.h
  * @author : panyao
  * @version:
  * @date   : 2021,11,03
  * @brief  : dac数据处理模块 头文件
  ******************************************************************************
  */

#ifndef __GEN_H__
#define __GEN_H__
#include "g.h"

extern int gen_init(void);
extern s32_t gen_data(u32_t offset, u8_t data);

#endif /*__GEN_H__*/
