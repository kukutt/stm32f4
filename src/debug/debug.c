/**
  ******************************************************************************
  * @file   : debug.c
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 调试功能
  ******************************************************************************
*/
#include "g.h"

s32_t debug_init(void){
    return 0;
}

#ifdef BOOTFLG
#define MAXUARTLEN 1200
#else
#define MAXUARTLEN 128
#endif
uint8_t saveuart[MAXUARTLEN+4];
uint16_t saveuarti=0;

int debug_loop(void){
    char g_uartData;
    if(/* ¿¿¿¿¿¿¿ */){
        /* ¿¿¿¿¿¿¿ */
        g_uartData = /* ¿¿¿¿ */
        if ((g_uartData == '\r') || (g_uartData == '\n') || (g_uartData == ';') || (saveuarti > MAXUARTLEN)){
            saveuart[saveuarti] = 0;
            printf("\r\n");
            
            if (0 == memcmp("test", saveuart, 4)){
#ifndef BOOTFLG
                printf("app_[%s]\r\n", APPVER);
#else
                printf("boot_[%s]\r\n", BOOTVER);
#endif
            }else if (0 == memcmp("reboot", saveuart, 7)){
                reboot();
#ifdef BOOTFLG
            }else if (0 == memcmp("iap_run", saveuart, 7)){
                IAP_UpdateCheck(1);
#endif
            }else if (0 == memcmp("iap_init", saveuart, 8)){
                /* flash¿¿¿¿¿ */
		printf("req:init ok");
            }else if (0 == memcmp("iap_read", saveuart, 8)){
                s8_t *p;
                u32_t addr = strtoul((char *)&saveuart[8], &p, 16);
                u32_t len = strtoul(p, &p, 10);
                printf("%08x %d\r\n", addr, len);
                printnameandhex("read", (char *)addr, len);
            }else if (0 == memcmp("iap_erase", saveuart, 9)){
                u32_t addr = strtoul((char *)&saveuart[9], NULL, 16);
                while(Ok != Flash_SectorErase(addr));
		/* flash¿¿ */
                printf("req:erase ok [%08x]\r\n", addr);
            }else if (0 == memcmp("iap_write", saveuart, 9)){
                s8_t buf[520];
                s8_t *p;
                s32_t i;
                s32_t errorflg = 0;
                u32_t addr = strtoul((char *)&saveuart[9], &p, 16);
                p+=1;
                u32_t len = strlen(p);
                u32_t tmp;
                HexStrToByte(p, (uint8_t *)buf, len);
                len = len / 2;
                for (i = 0; i<len; i+=4){
                    ((s8_t *)&tmp)[0] = buf[i+0];
                    ((s8_t *)&tmp)[1] = buf[i+1];
                    ((s8_t *)&tmp)[2] = buf[i+2];
                    ((s8_t *)&tmp)[3] = buf[i+3];
                    if (Ok != /* flash¿ */){
                        printf("req:write error [%d]\r\n", addr);
                        errorflg = 1;
                    }
                }
                if (0 == errorflg){
                    printf("req:write ok [%08x][%d]\r\n", addr, len);
                }
#ifndef BOOTFLG
            }else if (0 == memcmp("test", saveuart, 4)){
                printf("none\r\n");
#endif
            }else{
                printf("error\r\n");
                printnameandhex("hex", (char *)saveuart, saveuarti);
            }
            saveuarti=0;
        }else{
            saveuart[saveuarti] = g_uartData;
            saveuarti++;
        }
        
    }
    return 0;
}
