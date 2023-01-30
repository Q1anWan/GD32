/*
	Delay.c
	2022/11/20
	Version 1.0
	Systick延时
	支持STM32 LL或GD32 FW
*/
#include "Delay.h"

static uint32_t fac_us=0;							//us延时倍乘数

//初始化延迟函数
//SYSTICK的时钟固定为AHB时钟
//SYSCLK:系统时钟频率Mhz
void qDelay_init(uint32_t SYSCLK)
{
	#ifdef GD32_LIB
	SysTick->CTRL |= SYSTICK_CLKSOURCE_HCLK;
	#elif
	SysTick->CTRL |= LL_SYSTICK_CLKSOURCE_HCLK;
	#endif
	fac_us=SYSCLK;
	SysTick->VAL  = 0;
	SysTick->LOAD = 0xFFF;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}								    

//延时nus
//nus为要延时的us数.	
//nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)	 
void qDelay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	};
}

//延时nms
//nms:要延时的ms数
void qDelay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) qDelay_us(1000);
}

			 