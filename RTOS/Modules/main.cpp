#include "gd32f30x_it.h"
#include "main.h"
#include "arm_math.h"
#include <rthw.h>
#include "rtthread.h"

static rt_thread_t led1_thread = RT_NULL;
static void led1_thread_entry(void* parameter);

int main(void)
{
	rcu_periph_clock_enable(RCU_GPIOC);
	gpio_init(GPIOC,GPIO_MODE_OUT_PP,GPIO_OSPEED_MAX,GPIO_PIN_13);

	led1_thread =                          /* �߳̿��ƿ�ָ�� */
	rt_thread_create( "led1",              /* �߳����� */
										led1_thread_entry,   /* �߳���ں��� */
										RT_NULL,             /* �߳���ں������� */
										128,                 /* �߳�ջ��С */
										3,                   /* �̵߳����ȼ� */
										20);                 /* �߳�ʱ��Ƭ */
								 
	/* �����̣߳��������� */
  rt_thread_startup(led1_thread);
	return RT_EOK;
}


static void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		gpio_bit_set(GPIOC,GPIO_PIN_13);
		rt_thread_delay(500);   /* ��ʱ500��tick */
	  gpio_bit_reset(GPIOC,GPIO_PIN_13);
		rt_thread_delay(500);   /* ��ʱ500��tick */		 		
	}
}