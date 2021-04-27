/**
  ******************************************************************************
  * @file   : task_bsp.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 任务调度 底层功能接口封装
  ******************************************************************************
*/
#include "task.h"


uint32_t task_bsp_time(void){
    return /* bsp 返回1ms的计时器 */;
}

/* 检查是否超时,考虑溢出问题 */
int task_bsp_abs_check(u32_t a, u32_t basic){
    uint32_t ret = 0;
    if (a > basic){
        ret = a-basic;
    }else{
        ret = basic-a;
    }
    if (ret < 0x7fffffff){
        return 1;
    }else{
        return 0;
    }
}

void task_bsp_test(void){
    printf("hello\r\n");
    task_add(task_bsp_test, 5000);
}

