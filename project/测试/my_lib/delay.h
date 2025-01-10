/**
  ******************************************************************************
  * @file    delay.c
  * @author  铁头山羊
  * @version V 1.0.0
  * @date    2022年8月30日
  * @brief   延迟函数源头文件
  ******************************************************************************
  */

#ifndef _DELAY_H_
#define _DELAY_H_

#include "stm32f10x.h"

    void Delay_Init(void); // 延迟函数初始化
    void Delay(uint32_t ms); // 延迟
uint64_t GetTick(void); // 获取系统的当前时间

#endif
