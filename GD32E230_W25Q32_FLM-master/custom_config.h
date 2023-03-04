/*!
    \file    custom_config.h
    \brief   应用配置，包括组件使能，关键特性、参数的配置
    
    \version 2021-01-09, V1.0.0
*/

#ifndef __CUSTOM_CONFIG_H
#define __CUSTOM_CONFIG_H

#include "bsp_uart.h"

/* 使能LOG打印 */
#define USER_LOG_ENABLE  1

#if USER_LOG_ENABLE
#define log_printf(format, ...)  kprintf(format, ##__VA_ARGS__);  
#else
#define log_printf(format, ...)
#endif

#endif  /* __CUSTOM_CONFIG_H */