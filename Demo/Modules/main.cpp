#include "gd32f30x.h"
#include <stdio.h>
#include "main.h"
#include "Delay.h"
#include "arm_math.h"

#include "IIC_Software_GD.h"
#include "SPI_GD.h"
#include "UART_GD.h"
cIIC IIC_Test;
cSPI SPI_Test;
uint8_t Read=0;
int8_t a1[3]={1,2,3};
int8_t a2[3]={1,2,3};
volatile uint16_t uart_tx_flag=0;
volatile uint16_t uart_tx_flag2=0;
cUART UART_2;
void Tim_Init(void);
void UART_Init(void);
void SPI_Init(void);
uint8_t UART_RX_BUF_TEST[10]={0};
uint16_t UART_DMA_TX_St = 0;
uint8_t UART_TX_BUF[64]={0,0,0,0,0,0,12,3,22,12,100,200,30,221,69,5,10,78,68,1};
uint8_t UART_RX_BUF[1024]={0};
float signa1[6]={100,200,300,221,969,5};
float signa2[20]={0,0,0,0,0,0,12,3,22,12,100,200,300,221,969,5,10,78,683,1};
float signa3[39]={0};	
float maxdata;uint32_t maxindex;
uint32_t DMA_time_cost[5] = {0};
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
	
	Tim_Init();
	UART_Init();
	SPI_Init();
	
	
	UART_2.UART_Init(USART2,DMA0,DMA_CH2,DMA0,DMA_CH1);
    /* initilize the LEDs, USART and key */
    rcu_periph_clock_enable(LED_GPIO_CLK);
	rcu_periph_clock_enable(RCU_GPIOB);
	
	gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,LED_PIN);
	gpio_init(LED_GPIO_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_13);

//	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ,GPIO_PIN_5);
//	gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_10MHZ,GPIO_PIN_6);
	
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_3);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_4);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_5);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,GPIO_PIN_6);

	GPIO_BC(LED_GPIO_PORT) = LED_PIN;
	GPIO_BC(LED_GPIO_PORT) = GPIO_PIN_0;
    /* print out the clock frequency of system, AHB, APB1 and APB2 */
	//IIC_Test.IIC_Init(GPIOA,GPIO_PIN_5,GPIOA,GPIO_PIN_6);
	SPI_Test.SPI_Init(SPI1,GPIOB,GPIO_PIN_12,DMA0,DMA_CH3,DMA0,DMA_CH4);
	
	//UART_2.Recieve_DMA(UART_RX_BUF,768);
	
	arm_correlate_f32(signa1,6,signa2,20,signa3);
	arm_max_f32(signa3,40,&maxdata,&maxindex);
    for(;;)
	{
		arm_add_q7(a1,a2,a1,3);
		IIC_Test.IIC_Start();
		IIC_Test.IIC_SendOneByte(0X03);
		IIC_Test.IIC_SendOneByte(0X0B);
		IIC_Test.IIC_Start();
		IIC_Test.IIC_SendOneByte(0X0B);
		Read = IIC_Test.IIC_ReadOneByte();
		IIC_Test.IIC_Stop();
		
		SPI_Test.CS_0();
		SPI_Test.SPI_ExchangeOneByte(0x02);
		SPI_Test.CS_1();
    }
}
void TIMER2_IRQHandler(void)
{
	static uint16_t Beat;
	timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
	if(++Beat==1000){Beat=0;(GPIO_ISTAT(LED_GPIO_PORT)&(LED_PIN))?GPIO_BC(LED_GPIO_PORT) = LED_PIN:GPIO_BOP(LED_GPIO_PORT) = LED_PIN;}
	
	if(Beat%500==10)
	{
		UART_DMA_TX_St = UART_2.Transmit_DMA(UART_TX_BUF,40);
		UART_DMA_TX_St = UART_2.Transmit_DMA(UART_TX_BUF,40);
	}

	if(Beat%10==2)
	{
		
		TIMER_CH1CV(TIMER1) = (TIMER_CH1CV(TIMER1)>998)?0:TIMER_CH1CV(TIMER1)+2;
	}
}
void DMA0_Channel1_IRQHandler(void)
{
	UART_2.Transmit_IRQ();
}
void DMA0_Channel2_IRQHandler(void)
{
	DMA_time_cost[2] = timer_counter_read(TIMER3);
	if(UART_2.Recieve_IRQ())
	{
		DMA_time_cost[3] = timer_counter_read(TIMER3);
		UART_2.Recieve_DMA(UART_RX_BUF,768);
		DMA_time_cost[4] = timer_counter_read(TIMER3);
	}
}
void USART2_IRQHandler(void)
{
	timer_counter_value_config(TIMER3,0);
	DMA_time_cost[0] = timer_counter_read(TIMER3);
	UART_2.Recieve_IRQ();
	DMA_time_cost[1] = timer_counter_read(TIMER3);
	UART_2.Recieve_DMA(UART_RX_BUF,768);
	DMA_time_cost[2] = timer_counter_read(TIMER3);
}
void Tim_Init(void)
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
	
	/* Timer5中断设置，抢占优先级0，子优先级0 */
	nvic_irq_enable(TIMER2_IRQn, 1, 1); 
	/* 使能Timer5更新中断 */
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);
	/* 使能Timer5 */
	timer_enable(TIMER2);
	
	
	/*TIM1*/
	
	/* 开启时钟 */
	rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_AF);
	rcu_periph_clock_enable(RCU_TIMER1);
	/*初始化输出IO*/
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
	gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);
	
	
	timer_deinit(TIMER1);
	timer_init_struct.prescaler			= 0;	/* 预分频系数 */
	timer_init_struct.period			= 999;	/* 自动重装载值 */
	timer_init_struct.alignedmode		= TIMER_COUNTER_EDGE;	/* 计数器对齐模式，边沿对齐*/
	timer_init_struct.counterdirection	= TIMER_COUNTER_UP;		/* 计数器计数方向，向上*/
	timer_init_struct.clockdivision		= TIMER_CKDIV_DIV1;		/* DTS时间分频值 */
	timer_init_struct.repetitioncounter = 0;					/* 重复计数器的值*/
	timer_init(TIMER1, &timer_init_struct);
	
	/* CH0 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);
	/* CH1 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
    timer_channel_output_config(TIMER1,TIMER_CH_1,&timer_ocintpara);
	/* CH2 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
    timer_channel_output_config(TIMER1,TIMER_CH_2,&timer_ocintpara);
	/* CH3 configuration in PWM mode1 */
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;       //通道输出极性
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;            //通道输出状态
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;     //通道处于空闲时的输出	
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_LOW;     //互补通道输出极性 
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;          //互补通道输出状态
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;    //互补通道处于空闲时的输出
    timer_channel_output_config(TIMER1,TIMER_CH_3,&timer_ocintpara);
	
	/* CH configuration in TOGGLE mode */
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,499);
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_1,499);
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_2,499);
	timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_3,499);
	
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_1,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_2,TIMER_OC_MODE_PWM0);
	timer_channel_output_mode_config(TIMER1,TIMER_CH_3,TIMER_OC_MODE_PWM0);
	
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_1,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_2,TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config(TIMER1,TIMER_CH_3,TIMER_OC_SHADOW_DISABLE);

    /* TIMER7 primary output function enable 高级定时器（0和7）特有*/
    //timer_primary_output_config(TIMER1,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
	
	/*TIM3*/

	/* 开启定时器时钟 */
	rcu_periph_clock_enable(RCU_TIMER3);

	/* 初始化定时器 */
	timer_deinit(TIMER3);
	timer_init_struct.prescaler			= 11;	/* 预分频系数 */
	timer_init_struct.period			= 0xFFFF;	/* 自动重装载值 */
	timer_init_struct.alignedmode		= TIMER_COUNTER_EDGE;	/* 计数器对齐模式，边沿对齐*/
	timer_init_struct.counterdirection	= TIMER_COUNTER_UP;		/* 计数器计数方向，向上*/
	timer_init_struct.clockdivision		= TIMER_CKDIV_DIV1;		/* DTS时间分频值 */
	timer_init_struct.repetitioncounter = 0;					/* 重复计数器的值*/
	timer_init(TIMER3, &timer_init_struct);
	
	/* 使能Timer*/
	timer_enable(TIMER3);
}

void UART_Init(void)
{
	/*基本外设初始化*/
	/*初始化时钟*/
	rcu_periph_clock_enable(RCU_USART2);
	rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

	/*初始化GPIO*/
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
	gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);
	
	/* USART configure */
    usart_deinit(USART2);
    usart_baudrate_set(USART2,9600U);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);

	/*DMA初始化*/
		
	/*初始化时钟*/
	rcu_periph_clock_enable(RCU_DMA0);
	
	/*发送DMA初始化*/
	dma_parameter_struct dma_init_struct;
	dma_deinit(DMA0, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART2));
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH1);
	/*允许UART发送引起DMA请求*/
	usart_dma_transmit_config(USART2, USART_DENT_ENABLE);
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH1,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel1_IRQn, 1, 1);

	/*接收DMA初始化*/
	dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.periph_addr	= ((uint32_t)&USART_DATA(USART2));
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH2);
	/*允许UART接收引起DMA请求*/
	usart_dma_receive_config(USART2, USART_DENR_ENABLE);
	/*允许中断*/
	nvic_irq_enable(USART2_IRQn, 1, 1);
	nvic_irq_enable(DMA0_Channel2_IRQn, 1, 1);
}

void SPI_Init(void)
{
	/*初始化时钟*/
	rcu_periph_clock_enable(RCU_SPI1);
	rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_AF);

	/*初始化IO*/
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_15);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_14);
	gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_MAX, GPIO_PIN_13);
	gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX, GPIO_PIN_12);
	
	/*初始化SPI*/
	spi_parameter_struct  spi_init_struct;
	spi_i2s_deinit(SPI1);
	spi_quad_disable(SPI1);
	/* SPI0 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_8;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
	spi_init(SPI1, &spi_init_struct);
	
	/*初始化DMA*/
	/*发送DMA初始化*/
	dma_parameter_struct dma_init_struct;
	dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.periph_addr	= (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH3);
	/*禁止内存搬运*/
	dma_memory_to_memory_disable(DMA0, DMA_CH3);
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH3,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel3_IRQn, 1, 1);
	/*使能DMA*/
	spi_dma_enable(SPI1,SPI_DMA_TRANSMIT);
	
	/*接收DMA初始化*/
	dma_deinit(DMA0, DMA_CH4);
    dma_struct_para_init(&dma_init_struct);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
	dma_init_struct.periph_addr	=  (uint32_t)&SPI_DATA(SPI1);
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);
	/*禁止循环*/
	dma_circulation_disable(DMA0,DMA_CH4);
	/*禁止内存搬运*/
	dma_memory_to_memory_disable(DMA0, DMA_CH4);
	/*允许中断*/
	dma_interrupt_enable(DMA0,DMA_CH4,DMA_INT_FTF);
	nvic_irq_enable(DMA0_Channel4_IRQn, 1, 1);
	/*使能DMA*/
	spi_dma_enable(SPI1,SPI_DMA_RECEIVE);
}