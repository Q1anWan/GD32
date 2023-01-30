/*终极压力测试 328MHz 全外设*/

#include "gd32f30x.h"
#include <stdio.h>
#include "main.h"
#include "Delay.h"
#include "arm_math.h"

#include "SPI_GD.h"
#include "UART_GD.h"


void Timer_Init(void);
void UART_Init(void);
void SPI_Init(void);
void CAN_Init(void);

cUART UART_0;
cUART UART_1;
cUART UART_2;

cSPI SPI_0;
cSPI SPI_1;
cSPI SPI_2;

uint8_t UART_RX_BUF_TEST[10]={0};
uint16_t UART_DMA_TX_St = 0;
uint8_t UART_TX_BUF[64]={0,0,0,0,0,0,12,3,22,12,100,200,30,221,69,5,10,78,68,1};
uint8_t UART_RX_BUF[1024]={0};
can_receive_message_struct receive_message;
double A[10]={1.01223323,2.01223323,3.01223323,4.01223323,5.01223323,1212.01223323,32.01223323,1212121212.01223323,111.11111673411231};
double B[10]={1.01223323,2.01223323,3.01223323,4.01223323,5.01223323,1212.01223323,32.01223323,1212121212.01223323,111.11111673411231};
double C[10]={0};

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

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/

int main(void)
{
  /* configure systick */
	
  qDelay_init(360);
	rcu_periph_clock_enable(RCU_GPIOA);
	rcu_periph_clock_enable(RCU_GPIOB);
  rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_DMA0);
	rcu_periph_clock_enable(RCU_DMA1);
	UART_Init();
	SPI_Init();
	CAN_Init();
	
	UART_0.UART_Init(USART0,DMA0,DMA_CH4,DMA0,DMA_CH3);
	UART_1.UART_Init(USART1,DMA0,DMA_CH5,DMA0,DMA_CH6);
	//缺少DMA通道
	UART_2.UART_Init(USART2,DMA1,DMA_CH3,DMA1,DMA_CH4);

	SPI_0.SPI_Init(SPI0,GPIOA,GPIO_PIN_4,DMA0,DMA_CH1,DMA0,DMA_CH2);
	SPI_2.SPI_Init(SPI2,GPIOA,GPIO_PIN_15,DMA1,DMA_CH0,DMA1,DMA_CH1);
	//缺少DMA通道
	SPI_1.SPI_Init(SPI1,GPIOB,GPIO_PIN_12,DMA1,DMA_CH3,DMA1,DMA_CH4);
	
  /* initilize the LED */
  rcu_periph_clock_enable(LED_GPIO_CLK);
	rcu_periph_clock_enable(RCU_GPIOB);
	gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_13);
	
	GPIO_BC(LED_GPIO_PORT) = GPIO_PIN_0;
	
	Timer_Init();
	
	//UART_0.Transmit_DMA(UART_TX_BUF,64);
	UART_1.Transmit_DMA(UART_TX_BUF,64);
	UART_0.Recieve_DMA(UART_RX_BUF,64);
	UART_1.Recieve_DMA(UART_RX_BUF,64);
	UART_2.Transmit(UART_TX_BUF,64,100);
	SPI_0.Transmit_DMA(UART_TX_BUF,64);
	SPI_2.Transmit_DMA(UART_TX_BUF,64);

  for(;;)
	{
		for(uint32_t i =0;i<600000;i++){
		arm_mult_f64(A,B,C,10);}
  }
}
void TIMER2_IRQHandler(void)
{
	static uint16_t Beat;
	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
	if(++Beat==1000){Beat=0;(GPIO_ISTAT(LED_GPIO_PORT)&(LED_PIN))?GPIO_BC(LED_GPIO_PORT) = LED_PIN:GPIO_BOP(LED_GPIO_PORT) = LED_PIN;}
	CAN_Transmit(0x200,UART_TX_BUF);
	
//	UART_0.Transmit_DMA(UART_TX_BUF,64);
//	UART_1.Transmit_DMA(UART_TX_BUF,64);

}
void CAN0_RX1_IRQHandler(void)
{
	 can_message_receive(CAN0, CAN_FIFO1, &receive_message);
}
void DMA0_Channel1_IRQHandler(void)
{
	SPI_0.IRQ_Rx();
}
void DMA0_Channel2_IRQHandler(void)
{
	SPI_0.IRQ_Tx();
	SPI_0.Transmit_DMA(UART_TX_BUF,64);
}
void DMA0_Channel4_IRQHandler(void)
{
	UART_0.Recieve_IRQ();
	UART_0.Recieve_DMA(UART_RX_BUF,64);
}
void DMA0_Channel5_IRQHandler(void)
{
	UART_1.Recieve_IRQ();
	UART_1.Recieve_DMA(UART_RX_BUF,64);
}
void DMA1_Channel0_IRQHandler(void)
{
	SPI_2.IRQ_Rx();
}
void DMA1_Channel1_IRQHandler(void)
{
	SPI_2.IRQ_Tx();
	SPI_2.Transmit_DMA(UART_TX_BUF,64);
}
void DMA0_Channel3_IRQHandler(void)
{
	UART_0.Transmit_IRQ();
//UART_0.Transmit_DMA(UART_TX_BUF,64);
}
void DMA0_Channel6_IRQHandler(void)
{
	UART_1.Transmit_IRQ();
//UART_1.Transmit_DMA(UART_TX_BUF,64);
}
void USART0_IRQHandler(void)
{
	UART_0.Recieve_IRQ();
	UART_0.Recieve_DMA(UART_RX_BUF,64);
	UART_1.Transmit_DMA(UART_TX_BUF,64);
	UART_0.Transmit_DMA(UART_TX_BUF,64);
}
void USART1_IRQHandler(void)
{
	UART_1.Recieve_IRQ();
	UART_1.Recieve_DMA(UART_RX_BUF,64);
	UART_1.Transmit_DMA(UART_TX_BUF,64);
	UART_0.Transmit_DMA(UART_TX_BUF,64);
}
void Timer_Init(void)
{
	/* 定义定时器初始化结构体 */
	timer_parameter_struct timer_init_struct;
	timer_oc_parameter_struct timer_ocintpara;

	/* 开启定时器时钟 */
	rcu_periph_clock_enable(RCU_TIMER1);
	rcu_periph_clock_enable(RCU_TIMER2);

	rcu_periph_clock_enable(RCU_TIMER4);
	rcu_periph_clock_enable(RCU_TIMER5);
	rcu_periph_clock_enable(RCU_TIMER6);
	rcu_periph_clock_enable(RCU_TIMER7);

	
	/* 初始化定时器 */
	timer_deinit(TIMER1);
	timer_deinit(TIMER2);

	timer_deinit(TIMER4);
	timer_deinit(TIMER5);
	timer_deinit(TIMER6);
	timer_deinit(TIMER7);
	
	timer_init_struct.prescaler			= 328;	/* 预分频系数 */
	timer_init_struct.period			= 99;	/* 自动重装载值 */
	timer_init_struct.alignedmode		= TIMER_COUNTER_EDGE;	/* 计数器对齐模式，边沿对齐*/
	timer_init_struct.counterdirection	= TIMER_COUNTER_UP;		/* 计数器计数方向，向上*/
	timer_init_struct.clockdivision		= TIMER_CKDIV_DIV1;		/* DTS时间分频值 */
	timer_init_struct.repetitioncounter = 0;					/* 重复计数器的值*/
	
	timer_init(TIMER1, &timer_init_struct);
	timer_init(TIMER2, &timer_init_struct);

	timer_init(TIMER4, &timer_init_struct);
	timer_init(TIMER5, &timer_init_struct);
	timer_init(TIMER6, &timer_init_struct);
	timer_init(TIMER7, &timer_init_struct);
	
	
	/* Timer2中断设置，抢占优先级0，子优先级0 */
	nvic_irq_enable(TIMER2_IRQn, 1, 1); 
	/* 使能Timer2更新中断 */
  timer_interrupt_enable(TIMER2, TIMER_INT_UP);
	
	/* 使能Timer */
	timer_enable(TIMER1);


	timer_enable(TIMER4);
	timer_enable(TIMER5);
	timer_enable(TIMER6);
	timer_enable(TIMER7);
	
	/*TIM3*/
	
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_TIMER3);
	/*初始化输出IO*/
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_8);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);
	
	timer_deinit(TIMER3);
	timer_init_struct.prescaler			= 0;	/* 预分频系数 */
	timer_init_struct.period			= 9;	/* 自动重装载值 */
	timer_init_struct.alignedmode		= TIMER_COUNTER_EDGE;	/* 计数器对齐模式，边沿对齐*/
	timer_init_struct.counterdirection	= TIMER_COUNTER_UP;		/* 计数器计数方向，向上*/
	timer_init_struct.clockdivision		= TIMER_CKDIV_DIV1;		/* DTS时间分频值 */
	timer_init_struct.repetitioncounter = 0;					/* 重复计数器的值*/
	timer_init(TIMER3, &timer_init_struct);
	
/* CH0 configuration in PWM mode1 */
	timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
	timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
	timer_channel_output_config(TIMER3,TIMER_CH_0,&timer_ocintpara);
/* CH1 configuration in PWM mode1 */
	timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
	timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
	timer_channel_output_config(TIMER3,TIMER_CH_1,&timer_ocintpara);
/* CH2 configuration in PWM mode1 */
	timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
	timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
	timer_channel_output_config(TIMER3,TIMER_CH_2,&timer_ocintpara);
/* CH3 configuration in PWM mode1 */
	timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
	timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
	timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
	timer_channel_output_config(TIMER3,TIMER_CH_3,&timer_ocintpara);
	
	/* CH configuration in TOGGLE mode */
	timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_0,2);
	timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_1,2);
	timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_2,2);
	timer_channel_output_pulse_value_config(TIMER3,TIMER_CH_3,2);
	
  timer_channel_output_mode_config(TIMER3,TIMER_CH_0,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER3,TIMER_CH_1,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER3,TIMER_CH_2,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER3,TIMER_CH_3,TIMER_OC_MODE_PWM0);
	
  timer_channel_output_shadow_config(TIMER3,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER3,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER3,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER3,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);

	/* TIMER7 primary output function enable 高级定时器（0和7）特有*/
	//timer_primary_output_config(TIMER1,ENABLE);
	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable(TIMER3);
	timer_enable(TIMER3);
	timer_enable(TIMER2);
}

void UART_Init(void)
{
	/*基本外设初始化*/
	/*初始化时钟*/
	rcu_periph_clock_enable(RCU_USART0);
	rcu_periph_clock_enable(RCU_USART1);
	rcu_periph_clock_enable(RCU_USART2);


	/*初始化GPIO*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_9);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_8);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_3);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_10);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_MAX, GPIO_PIN_11);
	
	/* USART configure */
	usart_deinit(USART0);
  usart_deinit(USART1);
	usart_deinit(USART2);
	
  usart_baudrate_set(USART0,25600U); 
	usart_baudrate_set(USART1,25600U); 
	usart_baudrate_set(USART2,25600U);
	usart_receive_config(USART0, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	usart_receive_config(USART1, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
	usart_receive_config(USART2, USART_RECEIVE_ENABLE);
	usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);

	/*DMA初始化*/
	
	/*发送DMA初始化*/
	dma_parameter_struct dma_init_struct;
	
	dma_deinit(DMA0, DMA_CH3);
	dma_deinit(DMA0, DMA_CH6);
	
	dma_struct_para_init(&dma_init_struct);
	
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
  dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART0));
	dma_init(DMA0, DMA_CH3, &dma_init_struct);
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART1));
	dma_init(DMA0, DMA_CH6, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH3);
	dma_circulation_disable(DMA0,DMA_CH6);
	/*允许UART发送引起DMA请求*/
	usart_dma_transmit_config(USART0, USART_DENT_ENABLE);
	usart_dma_transmit_config(USART1, USART_DENT_ENABLE);
	
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH3,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel3_IRQn, 1, 1);
	dma_interrupt_enable(DMA0,DMA_CH6,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel6_IRQn, 1, 1);


	/*接收DMA初始化*/
	dma_deinit(DMA0, DMA_CH4);
	dma_deinit(DMA0, DMA_CH5);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART0));
  dma_init(DMA0, DMA_CH4, &dma_init_struct);
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART1));
  dma_init(DMA0, DMA_CH5, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH4);
	dma_circulation_disable(DMA0,DMA_CH5);
	/*允许UART接收引起DMA请求*/
	usart_dma_receive_config(USART0, USART_DENR_ENABLE);
	usart_dma_receive_config(USART1, USART_DENR_ENABLE);
	/*允许中断*/
	nvic_irq_enable(USART0_IRQn, 1, 1);
	nvic_irq_enable(DMA0_Channel4_IRQn, 1, 1);
	nvic_irq_enable(USART1_IRQn, 1, 1);
	nvic_irq_enable(DMA0_Channel5_IRQn, 1, 1);
}

void SPI_Init(void)
{
	/*初始化时钟*/
	rcu_periph_clock_enable(RCU_SPI0);
	rcu_periph_clock_enable(RCU_SPI1);
	rcu_periph_clock_enable(RCU_SPI2);

	/*初始化IO*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_6);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_7);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);
	
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);
	
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_3);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_4);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_5);
	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);
	
	/*初始化SPI*/
	spi_parameter_struct  spi_init_struct;
	spi_i2s_deinit(SPI0);
	spi_i2s_deinit(SPI1);
	spi_i2s_deinit(SPI2);
	spi_quad_disable(SPI0);

	/* SPI0 parameter config */
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.prescale             = SPI_PSC_8;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	

	spi_init(SPI0, &spi_init_struct);
	spi_init(SPI1, &spi_init_struct);
	spi_init(SPI2, &spi_init_struct);
	
	/*初始化DMA*/
	/*发送DMA初始化*/
	dma_parameter_struct dma_init_struct;
	dma_deinit(DMA0, DMA_CH2);
	dma_deinit(DMA1, DMA_CH1);
	dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	
	dma_init_struct.periph_addr	= (uint32_t)&SPI_DATA(SPI0);
	dma_init(DMA0, DMA_CH2, &dma_init_struct);
	dma_init_struct.periph_addr	= (uint32_t)&SPI_DATA(SPI2);
	dma_init(DMA1, DMA_CH1, &dma_init_struct);
	
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH2);
	dma_circulation_disable(DMA1,DMA_CH1);
	/*禁止内存搬运*/
	dma_memory_to_memory_disable(DMA0, DMA_CH2);
	dma_memory_to_memory_disable(DMA1, DMA_CH1);
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH2,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel2_IRQn, 1, 1);
	dma_interrupt_enable(DMA1,DMA_CH1,DMA_INT_FTF);
	nvic_irq_enable(DMA1_Channel1_IRQn, 1, 1);
	/*使能DMA*/
	spi_dma_enable(SPI0,SPI_DMA_TRANSMIT);
	spi_dma_enable(SPI2,SPI_DMA_TRANSMIT);
	
	/*接收DMA初始化*/
	dma_deinit(DMA0, DMA_CH1);
	dma_deinit(DMA1, DMA_CH0);
  dma_struct_para_init(&dma_init_struct);
	dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
	dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
	dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
	dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
	dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
	
	dma_init_struct.periph_addr	=  (uint32_t)&SPI_DATA(SPI0);
	dma_init(DMA0, DMA_CH1, &dma_init_struct);
	dma_init_struct.periph_addr	=  (uint32_t)&SPI_DATA(SPI2);
	dma_init(DMA1, DMA_CH0, &dma_init_struct);	
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH1);
	dma_circulation_disable(DMA1,DMA_CH0);
	/*禁止内存搬运*/
	dma_memory_to_memory_disable(DMA0, DMA_CH1);
	dma_memory_to_memory_disable(DMA1, DMA_CH0);
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH1,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel1_IRQn, 1, 1);
	dma_interrupt_enable(DMA1,DMA_CH0,DMA_INT_FTF);
	nvic_irq_enable(DMA1_Channel0_IRQn, 1, 1);
	/*使能DMA*/
	spi_dma_enable(SPI0,SPI_DMA_RECEIVE);
	spi_dma_enable(SPI1,SPI_DMA_RECEIVE);
}

void CAN_Init(void)
{
	can_parameter_struct can_parameter;
	can_filter_parameter_struct can_filter;

	/* enable CAN clock */	
	rcu_periph_clock_enable(RCU_CAN0);
	/* configure CAN0 GPIO */
	gpio_init(GPIOA,GPIO_MODE_IPU,GPIO_OSPEED_50MHZ,GPIO_PIN_11);
	gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_50MHZ,GPIO_PIN_12);
	
	can_struct_para_init(CAN_INIT_STRUCT, &can_parameter);
	can_struct_para_init(CAN_INIT_STRUCT, &can_filter);
	/* initialize CAN register */
	can_deinit(CAN0);
	
	/* initialize CAN parameters */
	can_parameter.time_triggered = DISABLE;
	can_parameter.auto_bus_off_recovery = DISABLE;
	can_parameter.auto_wake_up = DISABLE;
	can_parameter.auto_retrans = DISABLE;
	can_parameter.rec_fifo_overwrite = DISABLE;
	can_parameter.trans_fifo_order = DISABLE;
	can_parameter.working_mode = CAN_NORMAL_MODE;
	can_parameter.resync_jump_width = CAN_BT_SJW_1TQ;
	can_parameter.time_segment_1 = CAN_BT_BS1_2TQ;
	can_parameter.time_segment_2 = CAN_BT_BS2_1TQ;
	
	/* 1MBps */
	// 328/2/41=4
	can_parameter.prescaler = 41;

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
