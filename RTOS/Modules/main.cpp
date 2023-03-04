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

	led1_thread =                          /* 线程控制块指针 */
	rt_thread_create( "led1",              /* 线程名字 */
										led1_thread_entry,   /* 线程入口函数 */
										RT_NULL,             /* 线程入口函数参数 */
										128,                 /* 线程栈大小 */
										3,                   /* 线程的优先级 */
										20);                 /* 线程时间片 */
								 
	/* 启动线程，开启调度 */
  rt_thread_startup(led1_thread);
	return RT_EOK;
}


static void led1_thread_entry(void* parameter)
{	
	while (1)
	{
		gpio_bit_set(GPIOC,GPIO_PIN_13);
		rt_thread_delay(500);   /* 延时500个tick */
	  gpio_bit_reset(GPIOC,GPIO_PIN_13);
		rt_thread_delay(500);   /* 延时500个tick */		 		
	}
}