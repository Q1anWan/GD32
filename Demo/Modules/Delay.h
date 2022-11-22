/*
	Delay.h
	2022/11/20
	Version 1.0
	Systick��ʱ
	֧��STM32 LL��GD32 FW
*/
#ifndef DELAY_H
#define DELAY_H
#include "main.h"
#ifdef __cplusplus
extern "C"
{
#endif
void qDelay_init(uint8_t SYSCLK);
void qDelay_ms(uint16_t nms);
void qDelay_us(uint32_t nus);
#ifdef __cplusplus
}
#endif
#endif
