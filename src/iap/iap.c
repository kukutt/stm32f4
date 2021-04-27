/**
  ******************************************************************************
  * @file   : iap.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : iap升级功能
  ******************************************************************************
*/
#include "g.h"

u32_t JumpAddress;
typedef void (*func_ptr_t)(void);
func_ptr_t JumpToApplication;
static bool_t IAP_JumpToApp(u32_t u32Addr)
{
    u32_t u32StackTop = *((__IO u32_t *)u32Addr);  //读取APP程序栈顶地址

    /* 判断栈顶地址有效性 */
    if ((u32StackTop > SRAM_START) && (u32StackTop <= (SRAM_END))){
        /* 配置跳转到用户程序复位中断入口 */
        JumpAddress = *(__IO u32_t *)(u32Addr + 4);
        JumpToApplication = (func_ptr_t)JumpAddress;
        /* 初始化用户程序的栈顶指针 */
        __set_MSP(*(__IO u32_t *)u32Addr);
        JumpToApplication();
    }

    return false;
}

void IAP_UpdateCheck(u32_t checkflg)
{
    u32_t u32AppFlag;
    
    u32AppFlag = *(__IO u32_t *)BOOT_PARA_ADDRESS; //读出BootLoader para区标记值
    if ((checkflg==1) || (APP_FLAG != u32AppFlag))                       //如果标记值不等于APP_FLAG,表示不需要升级APP程序
    {
        IAP_JumpToApp(APP_ADDRESS);                   //则直接跳转至APP
    }    
}
