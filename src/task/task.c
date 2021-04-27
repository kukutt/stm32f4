/**
  ******************************************************************************
  * @file   : task.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 任务调度功能
  ******************************************************************************
*/
#include "task.h"

extern u32_t task_bsp_time(void);
extern s32_t task_bsp_abs_check(u32_t a, u32_t b);
extern void task_bsp_test(void);

typedef struct _task_T
{
    void (*fn)(void);
    u32_t timeout;
    u8_t addcount;
    u8_t runcount;
    u8_t res[2];
}task_T;
#define MAXTASKNUM 32
task_T g_task[MAXTASKNUM];

s32_t task_init(void){
    memset((char *)&g_task, 0, sizeof(g_task));
    //task_add(task_bsp_test, 5000);
    return 0;
}

s32_t task_loop(void){
    int i;
    u32_t nowtime = task_bsp_time();
    
    for (i = 0; i < MAXTASKNUM; i++){
        if (g_task[i].runcount != g_task[i].addcount){
            //printf("[task_run]%d %d %d\r\n", i, g_task[i].runcount, g_task[i].addcount);
            if ((g_task[i].timeout <= nowtime) && (task_bsp_abs_check(g_task[i].timeout, nowtime))){
                g_task[i].fn();
                g_task[i].runcount = g_task[i].addcount;
            }
        }
    }
    return 0;
}

s32_t task_add(void (*fn)(void), u32_t timeout){
    s32_t i = 0;
    
    for (i = 0; i < MAXTASKNUM; i++){
        if (g_task[i].runcount == g_task[i].addcount){
            g_task[i].fn = fn;
            g_task[i].timeout = timeout+task_bsp_time();
            //printf("[task_add]%d %08x\r\n", i, g_task[i].timeout);
            g_task[i].addcount++;
            break;
        }
    }
    return 0;
}
