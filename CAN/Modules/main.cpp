#include "gd32f30x.h"
#include <stdio.h>
#include "main.h"
#include "Delay.h"
#include "arm_math.h"

#include "IIC_Software_GD.h"
#include "SPI_GD.h"
#include "UART_GD.h"

#include "QCS_Lite.h"
#include "Data_Exchange.h"


void Timer_Init(void);
void can_gpio_config(void);
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter);
void CAN_Transmit(uint32_t ID,uint8_t *data);

can_parameter_struct can_init_parameter;
can_filter_parameter_struct can_filter_parameter;
can_receive_message_struct receive_message;

float IMU_Q[4]={0};
float IMU_Angle[3]={0};
uint8_t IMU_Update = 0; 

uint8_t data_buff[8]={0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
  /* configure systick */
	
  qDelay_init(120);
	
  /* initilize the LEDs, USART and key */
  rcu_periph_clock_enable(LED_GPIO_CLK);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_13);
	GPIO_BC(LED_GPIO_PORT) = LED_PIN;
	
	can_gpio_config();
	can_config(can_init_parameter, can_filter_parameter);
	
	Timer_Init();
  for(;;)
	{
		CAN_Transmit(0x100,data_buff);
		qDelay_ms(100);
	}
}
void TIMER2_IRQHandler(void)
{
	static uint16_t Beat;
	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
	
	if(++Beat==1000)
	{
		Beat=0;
		// LED BLINK
		(GPIO_ISTAT(LED_GPIO_PORT)&(LED_PIN))?GPIO_BC(LED_GPIO_PORT) = LED_PIN:GPIO_BOP(LED_GPIO_PORT) = LED_PIN;
	}

	if(Beat%10==0)
	{
		if(IMU_Update)
		{
			QCS_Show_Degree(IMU_Q,IMU_Angle);
			IMU_Update = 0;
		}
	}
	
	if(Beat%2==0)
	{
		
	}
}

void Timer_Init(void)
{
	/* 定义定时器初始化结构体 */
	timer_parameter_struct timer_init_struct;
	timer_oc_parameter_struct timer_ocintpara;
	
	/*TIM2*/
	/* 开启定时器时钟 */
	rcu_periph_clock_enable(RCU_TIMER2);

	/* 初始化定时器 */
	timer_deinit(TIMER2);
	timer_init_struct.prescaler			= 119;	/* 预分频系数 */
	timer_init_struct.period			= 999;	/* 自动重装载值 */
	timer_init_struct.alignedmode		= TIMER_COUNTER_EDGE;	/* 计数器对齐模式，边沿对齐*/
	timer_init_struct.counterdirection	= TIMER_COUNTER_UP;		/* 计数器计数方向，向上*/
	timer_init_struct.clockdivision		= TIMER_CKDIV_DIV1;		/* DTS时间分频值 */
	timer_init_struct.repetitioncounter = 0;					/* 重复计数器的值*/
	timer_init(TIMER2, &timer_init_struct);
	
	/* Timer2中断设置，抢占优先级0，子优先级0 */
	nvic_irq_enable(TIMER2_IRQn, 1, 1); 
	/* 使能Timer2更新中断 */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
	/* 使能Timer2 */
	timer_enable(TIMER2);
}

void can_gpio_config(void)
{
    /* enable CAN clock */
    rcu_periph_clock_enable(RCU_CAN0);
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
    
    /* configure CAN0 GPIO */
    gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
    
}
void can_config(can_parameter_struct can_parameter, can_filter_parameter_struct can_filter)
{
    can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
    can_struct_para_init(CAN_INIT_STRUCT, &can_filter);
    /* initialize CAN register */
    can_deinit(CAN0);
    
    /* initialize CAN parameters */
    can_parameter.time_triggered = DISABLE;
    can_parameter.auto_bus_off_recovery = DISABLE;
    can_parameter.auto_wake_up = DISABLE;
    can_parameter.auto_retrans = ENABLE;
    can_parameter.rec_fifo_overwrite = DISABLE;
    can_parameter.trans_fifo_order = DISABLE;
    can_parameter.working_mode = CAN_NORMAL_MODE;
    can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
    can_parameter.time_segment_1 = CAN_BT_BS1_5TQ;
    can_parameter.time_segment_2 = CAN_BT_BS2_4TQ;
    
    /* 1MBps */

    can_parameter.prescaler = 6;

    /* initialize CAN */
    can_init(CAN0, &can_parameter);
    
    /* initialize filter */ 
    can_filter.filter_number=0;
    can_filter.filter_mode = CAN_FILTERMODE_MASK;
    can_filter.filter_bits = CAN_FILTERBITS_32BIT;
    can_filter.filter_list_high = 0x0000;
    can_filter.filter_list_low = 0x0000;
    can_filter.filter_mask_high = 0x0000;
    can_filter.filter_mask_low = 0x0000;
    can_filter.filter_fifo_number = CAN_FIFO1;
    can_filter.filter_enable = ENABLE;
    
    can_filter_init(&can_filter);
		
		nvic_irq_enable(CAN0_RX1_IRQn,0,0);
		can_interrupt_enable(CAN0, CAN_INT_RFNE1);
}
void CAN0_RX1_IRQHandler(void)
{
	 can_message_receive(CAN0, CAN_FIFO1, &receive_message);
		if(receive_message.rx_sfid == 0x322)
		{
			Transform.U8_To_Float(receive_message.rx_data,IMU_Q,8);
		}
		else if(receive_message.rx_sfid == 0x323)
		{
			Transform.U8_To_Float(receive_message.rx_data,IMU_Q+2,8);
			IMU_Update = 1;
		}
}
void CAN_Transmit(uint32_t ID,uint8_t *data)
{
		can_trasnmit_message_struct transmit_message;
	
	  transmit_message.tx_sfid = ID;
    transmit_message.tx_efid = 0x00;
    transmit_message.tx_ft = CAN_FT_DATA;
    transmit_message.tx_ff = CAN_FF_STANDARD;
    transmit_message.tx_dlen = 8;
		
		transmit_message.tx_data[0] = data[0];
		transmit_message.tx_data[1] = data[1];
		transmit_message.tx_data[2] = data[2];
		transmit_message.tx_data[3] = data[3];
		transmit_message.tx_data[4] = data[4];
		transmit_message.tx_data[5] = data[5];
		transmit_message.tx_data[6] = data[6];
		transmit_message.tx_data[7] = data[7];
	
	  can_message_transmit(CAN0, &transmit_message);
}