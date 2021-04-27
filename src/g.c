/**
  ******************************************************************************
  * @file   : g.h
  * @author : panyao
  * @version:
  * @date   : 2021,4,27
  * @brief  : 全局功能
  ******************************************************************************
*/
#include "g.h"

void printnameandhex(s8_t *name, s8_t *str, s32_t len){
    s32_t i;
    s8_t *nametemp = name;
    s8_t *strtemp = str;

    printf("[%s]len=%d : ", nametemp, len);
    for (i = 0; i < len; i++)
    {
        printf("%02x", (unsigned char)strtemp[i]);
    }
    printf("\r\n");
}


/* 字节流转换为十六进制字符串 */
void ByteToHexStr(const s8_t* source, s8_t* dest, s32_t sourceLen){
    s16_t i;
    u8_t highByte, lowByte;
    int sourceLentemp = sourceLen;
    if (sourceLentemp == 0)
    {
        sourceLentemp = strlen((char *)source);
    }

    for (i = 0; i < sourceLentemp; i++){
        highByte = source[i] >> 4;
        lowByte = source[i] & 0x0f ;

        highByte += 0x30;

        if (highByte > 0x39)
                dest[i * 2] = highByte + 0x07;
        else
                dest[i * 2] = highByte;

        lowByte += 0x30;
        if (lowByte > 0x39)
            dest[i * 2 + 1] = lowByte + 0x07;
        else
            dest[i * 2 + 1] = lowByte;
    }
    dest[i*2] = 0;
    return ;
}

/* 十六进制字符串转换为字节流 */
void HexStrToByte(const s8_t* source, s8_t* dest, s32_t sourceLen){
    s32_t i;  
    u8_t highByte, lowByte;
    s32_t sourceLentemp = sourceLen;

    if (sourceLentemp == 0)
    {
        sourceLentemp = strlen((char *)source);
    }
      
    for (i = 0; i < sourceLentemp; i += 2)  
    {  
        highByte = source[i];  
        lowByte  = source[i + 1];  

        if (highByte >= 0x61) 
        {
            highByte -= 0x57;  
        }
        else if (highByte >= 0x41) 
        {
            highByte -= 0x37;  
        }
        else
        {
            highByte -= 0x30;  
        }
  
        if (lowByte >= 0x61) 
        {
            lowByte -= 0x57;  
        }
        else if (lowByte >= 0x41) 
        {
            lowByte -= 0x37;  
        }
        else
        {
            lowByte -= 0x30;  
        }
  
        dest[i / 2] = ((highByte << 4) & 0xf0) | (lowByte & 0x0f);  
    }  
    return ;  
} 

int float2int_round(f32_t in){
    return (int)(in+0.5f);
}


void delay_ms_c(u32_t ms){
    u32_t i;
    u32_t j;
    for(i=0; i<ms; i++)
        for(j=0; j<5980;j++);
}

void delay_us_c(u32_t i){
   u32_t j,k;
   
   for (j = 0; j <1; j++){
        for (k = 0; k < i; k++);
   }
}

void reboot(void){
    __NVIC_SystemReset();
}


u32_t time_delay_ms(u32_t timeout){
    return HAL_GetTick()+timeout;
}

/* 判断超时 */
bool_t time_delay_ms_check(u32_t time){
    u32_t ret = 0;
    u32_t now = HAL_GetTick();
    if (now > time){
        ret = now-time;
    }else{
        ret = time-now;
    }
    if (ret < 0x7fffffff){
        if (now >= time){
            return true;
        }else{
            return false;
        }
    }else{
        /* 溢出 */
        return false;
    }
}

