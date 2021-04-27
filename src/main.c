/**
  ******************************************************************************
  * @file   : main.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 主函数
  ******************************************************************************
*/
#include "g.h"

int test_time(void);
void sysclk_init(void);

int main(void){
    debug_init();
#ifndef BOOTFLG
    task_init();

    delay1ms(1);
    printf("app_start\r\n");
#else
    IAP_UpdateCheck(0);
    delay1ms(1);
    printf("boot\r\n");
#endif
    while(1){
        debug_loop();
#ifndef BOOTFLG
#endif
    }
}

