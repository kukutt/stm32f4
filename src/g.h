/**
  ******************************************************************************
  * @file   : g.h
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 全局功能 头文件
  ******************************************************************************
*/

#ifndef __G_H__
#define __G_H__

#define BOOTVER "V0.8.8"
#define APPVER  "V0.8.8"

/* 导入系统头文件 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef unsigned char u8_t;
typedef char s8_t;
typedef unsigned short u16_t;
typedef signed short s16_t;
typedef unsigned int u32_t;
typedef uint64_t u64_t;
typedef signed int s32_t;
typedef float f32_t;
typedef double f64_t;
typedef signed int err_t;
typedef unsigned char bool_t;

#define true 1
#define false 0

/* 导入自定义头文件 */
#include "iap/iap.h"
#include "task/task.h"
#include "debug/debug.h"


void printnameandhex(s8_t *name, s8_t *str, s32_t len);
void ByteToHexStr(const s8_t* source, s8_t* dest, s32_t sourceLen);
void HexStrToByte(const s8_t* source, s8_t* dest, s32_t sourceLen);
int  float2int_round(f32_t in);
void delay_ms_c(u32_t ms);
void delay_us_c(u32_t i);
void reboot(void);
u32_t time_delay_ms(u32_t timeout);
bool_t time_delay_ms_check(u32_t time);

#endif /* __G_H__ */
