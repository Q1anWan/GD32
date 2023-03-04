/*!
    \file    custom_config.h
    \brief   Ӧ�����ã��������ʹ�ܣ��ؼ����ԡ�����������
    
    \version 2021-01-09, V1.0.0
*/

#ifndef __CUSTOM_CONFIG_H
#define __CUSTOM_CONFIG_H

#include "bsp_uart.h"

/* ʹ��LOG��ӡ */
#define USER_LOG_ENABLE  1

#if USER_LOG_ENABLE
#define log_printf(format, ...)  kprintf(format, ##__VA_ARGS__);  
#else
#define log_printf(format, ...)
#endif

#endif  /* __CUSTOM_CONFIG_H */