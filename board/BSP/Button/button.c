/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtdevice.h>
#include "button.h"

/****    Debug     ****/
#define DBG_ENABLE
#define DBG_SECTION_NAME "button"
#define DBG_LEVEL DBG_INFO
#define DBG_COLOR
#include <rtdbg.h>

#define MY_BUTTON_CALL(func, argv) \
    do                             \
    {                              \
        if ((func) != RT_NULL)     \
            func argv;             \
    } while (0)

struct my_button_manage
{
    rt_uint8_t num;                                    // 管理到的按键数量
    rt_timer_t timer;                                  // 用于定时扫描按键状态的定时器句柄。
    struct my_button *button_list[MY_BUTTON_LIST_MAX]; // 按键特征的结构体
};

static struct my_button_manage button_manage;

int my_button_register(struct my_button *button)
{
    if (button->press_logic_level == 0)
    {
        rt_pin_mode(button->pin, PIN_MODE_INPUT_PULLUP);//非按下状态为高电平
    }
    else
    {
        rt_pin_mode(button->pin, PIN_MODE_INPUT_PULLDOWN);//非按下状态为低电平
    }

    button->cnt = 0;
    button->event = BUTTON_EVENT_NONE;
    button_manage.button_list[button_manage.num++] = button; // 按键结构体指针添加到按键列表中，并增加已注册按键的数量。

    return 0;
}

static void my_button_scan(void *param)
{
    rt_uint8_t i;
    rt_uint16_t cnt_old; // 保存上次按键的按下计数值

    for (i = 0; i < button_manage.num; i++) // 给所有注册的按键扫描一遍
    {
        cnt_old = button_manage.button_list[i]->cnt;

        if (rt_pin_read(button_manage.button_list[i]->pin) == button_manage.button_list[i]->press_logic_level) // 有按键按下
        {
            button_manage.button_list[i]->cnt++;

            if (button_manage.button_list[i]->cnt == MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_DOWN */
            {
                LOG_D("BUTTON_DOWN");
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_DOWN;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (button_manage.button_list[i]->cnt == MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD */
            {
                LOG_D("BUTTON_HOLD");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (button_manage.button_list[i]->cnt > MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_CYC */
            {
                LOG_D("BUTTON_HOLD_CYC");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_CYC;
                if (button_manage.button_list[i]->hold_cyc_period && button_manage.button_list[i]->cnt % (button_manage.button_list[i]->hold_cyc_period / MY_BUTTON_SCAN_SPACE_MS) == 0)
                    MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
        else
        {
            button_manage.button_list[i]->cnt = 0;                                                                               // 没有按键按下的状态，原本扫描次数计数归零
            if (cnt_old >= MY_BUTTON_DOWN_MS / MY_BUTTON_SCAN_SPACE_MS && cnt_old < MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_CLICK_UP */
            {                                                                                                                    // 之前按下的时间不算长也不算短，说明是短时间按了一下就松手了
                LOG_D("BUTTON_CLICK_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_CLICK_UP;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
            else if (cnt_old >= MY_BUTTON_HOLD_MS / MY_BUTTON_SCAN_SPACE_MS) /* BUTTON_HOLD_UP */
            {                                                                // 之前按下的时间比较长，说明是长按后松手
                LOG_D("BUTTON_HOLD_UP");
                button_manage.button_list[i]->event = BUTTON_EVENT_HOLD_UP;
                MY_BUTTON_CALL(button_manage.button_list[i]->cb, (button_manage.button_list[i]));
            }
        }
    }
}

int my_button_start()
{
    if (button_manage.timer != RT_NULL)
        return -1;

    /* 创建定时器1 */
    button_manage.timer = rt_timer_create("timer1",                                            /* 定时器名字是 timer1 */
                                          my_button_scan,                                      /* 超时时回调的处理函数 */
                                          RT_NULL,                                             /* 超时函数的入口参数 */
                                          RT_TICK_PER_SECOND * MY_BUTTON_SCAN_SPACE_MS / 1000, /* 定时长度，以OS Tick为单位，即10个OS Tick */
                                          RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);  /* 周期性定时器 */
                                                                                               /* 启动定时器 */
    if (button_manage.timer == RT_NULL)
    {
        LOG_E("Failed to create timer!");
        return -1;
    }

    if (rt_timer_start(button_manage.timer) != RT_EOK)
    {
        LOG_E("Failed to start timer!");
        rt_timer_delete(button_manage.timer);
        button_manage.timer = RT_NULL;
        return -1;
    }

    return 0;
}
