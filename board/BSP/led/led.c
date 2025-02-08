/*
* Copyright (c) 2006-2018, RT-Thread Development Team
*
* SPDX-License-Identifier: Apache-2.0
*
* Change Logs:
* Date           Author       Notes
* 2018-10-17     flybreak      the first version
*/

#include <rtdevice.h>

#include "led.h"

int led_init(void)
{
    /* 设定 LED 引脚为推挽输出模式 */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
		rt_pin_mode(LED1_PIN, PIN_MODE_OUTPUT);
    return 0;
}

int led0_on(void)
{
    /* 调用 API 输出低电平 */
    rt_pin_write(LED0_PIN, PIN_LOW);

    return 0;
}

int led0_off(void)
{
    /* 调用 API 输出高电平 */
    rt_pin_write(LED0_PIN, PIN_HIGH);

    return 0;
}

int led0_toggle(void)
{
    /* 调用 API 读出当前电平 然后输出相反电平 */
    rt_pin_write(LED0_PIN, !rt_pin_read(LED0_PIN));

    return 0;
}

int led1_on(void)
{
    /* 调用 API 输出低电平 */
    rt_pin_write(LED1_PIN, PIN_LOW);

    return 0;
}

int led1_off(void)
{
    /* 调用 API 输出高电平 */
    rt_pin_write(LED1_PIN, PIN_HIGH);

    return 0;
}
int led1_toggle(void)
{
    /* 调用 API 读出当前电平 然后输出相反电平 */
    rt_pin_write(LED1_PIN, !rt_pin_read(LED1_PIN));

    return 0;
}
