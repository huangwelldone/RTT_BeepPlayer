/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2018-10-17     flybreak      the first version
*/

#ifndef LED_H
#define LED_H
#include <drv_gpio.h>

#define LED0_PIN    GET_PIN(F, 9)
#define LED1_PIN    GET_PIN(F, 10)

int led_init(void);                     //LED 灯初始化
int led0_on(void);                       //LED 灯亮
int led0_off(void);                      //LED 灯灭
int led0_toggle(void);                   //LED 灯亮灭状态翻转
int led1_on(void);                       //LED 灯亮
int led1_off(void);                      //LED 灯灭
int led1_toggle(void);                   //LED 灯亮灭状态翻转

#endif
