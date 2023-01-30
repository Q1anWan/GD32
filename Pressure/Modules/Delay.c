/*
	Delay.c
	2022/11/20
	Version 1.0
	Systick��ʱ
	֧��STM32 LL��GD32 FW
*/
#include "Delay.h"

static uint32_t fac_us=0;							//us��ʱ������

//��ʼ���ӳٺ���
//SYSTICK��ʱ�ӹ̶�ΪAHBʱ��
//SYSCLK:ϵͳʱ��Ƶ��Mhz
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

//��ʱnus
//nusΪҪ��ʱ��us��.	
//nus:0~190887435(���ֵ��2^32/fac_us@fac_us=22.5)	 
void qDelay_us(uint32_t nus)
{		
	uint32_t ticks;
	uint32_t told,tnow,tcnt=0;
	uint32_t reload=SysTick->LOAD;			//LOAD��ֵ	    	 
	ticks=nus*fac_us; 						//��Ҫ�Ľ�����
	told=SysTick->VAL;        				//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}

//��ʱnms
//nms:Ҫ��ʱ��ms��
void qDelay_ms(uint16_t nms)
{
	uint32_t i;
	for(i=0;i<nms;i++) qDelay_us(1000);
}

			 