/*********************************************************************************
  *FileName:	UART_GD.cpp
  *Author:  	qianwan
  *Version:  	1.0
  *Date:  		2022/11/21
  *Description: GD32 串口发送基类
  *Other:		使用宏定义开启DMA收发
  *Other:		使用DMA发送时，还需额外在发送DMA的中断函数内调用Transmit_IRQ函数
  *Other:		使用DMA接收时，还需额外在接收DMA的中断函数、串口中断函数内调用Recieve_IRQ()函数
**********************************************************************************/
#include "UART_GD.h"
#include "string.h"


/*! 
 *  @brief      串口阻塞发送函数
 *  @param  	传入发送数组指针、数据数目
  *  @return  	发送正常返回0 | 发送异常返回USART_FLAG_BSY
 */
uint16_t cUART::Transmit(uint8_t *DT,uint16_t num,uint32_t OVT)
{
	uint16_t wait_cnt = 0;
	for (uint16_t i = 0; i < num; i++)
	{
		wait_cnt = 0;
		
		while (!(USART_REG_VAL(this->UART, USART_FLAG_TBE) & BIT(USART_BIT_POS(USART_FLAG_TBE))))
		{	this->Delay(1);
			if (wait_cnt++ > TIME_OVER_US)return USART_FLAG_BSY;
		}
		USART_DATA(this->UART) = ((uint16_t)USART_DATA_DATA & DT[i]);
	}
	wait_cnt = 0;
	while (!(USART_REG_VAL(this->UART, USART_FLAG_TBE) & BIT(USART_BIT_POS(USART_FLAG_TBE))))
	{
		this->Delay(1);
		if(wait_cnt++ > OVT)return USART_FLAG_BSY;
	}
	return 0;
}

/*! 
 *  @brief      串口阻塞发送函数
 *  @param  	传入发送数组指针、数据数目、超时us
 *  @return  	发送正常返回0 | 发送异常返回USART_FLAG_RT
 */
uint16_t cUART::Recieve(uint8_t *DT,uint16_t num,uint32_t OVT)
{
	uint16_t wait_cnt = 0;
	uint16_t i = 0;

	/*数据循环接受*/
	for (; i < num; i++)
	{
		/*等待数据并记录超时*/
		while(!(USART_REG_VAL(this->UART, USART_FLAG_RBNE) & BIT(USART_BIT_POS(USART_FLAG_RBNE))))
		{ 
			this->Delay(1);
			if(wait_cnt++>OVT)
			{
				this->Recieve_Length = i;
				return USART_FLAG_RT;
			}
		}
		
		/*RBNE时接收数据，此操作将清楚RBNE标志*/
		DT[i]=(uint16_t)(GET_BITS(USART_DATA(this->UART), 0U, 8U));
		wait_cnt = 0;
	}
	
	this->Recieve_Length = i;
	return 0;
}
#if (UART_USE_TX_DMA==1)
/*! 
 *  @brief   	UART DMA发送函数
 *  @param  	发送数组指针，发送字节长度
 *  @return  	发送正常返回0 | 发送异常返回USART_FLAG_BSY
 *  @other  	正常发挥功能需要在发送DMA的中断函数内调用Transmit_IRQ函数
 */
uint16_t cUART::Transmit_DMA(uint8_t *pData, uint16_t length)
{
	/*正在执行发送，返回串口忙*/
	if(!usart_flag_get(this->UART,USART_FLAG_TBE))
	{return USART_FLAG_BSY;}

	/*配置本次传输*/
	dma_interrupt_flag_clear(this->DMAt,this->DMA_CHt,DMA_INT_FLAG_FTF);
	dma_memory_address_config(this->DMAt,this->DMA_CHt,(uint32_t)pData);
	dma_transfer_number_config(this->DMAt,this->DMA_CHt,(uint32_t)length);
	/*使能传输*/
	dma_channel_enable(this->DMAt,this->DMA_CHt);
	return 0;
}
#endif

#if (UART_USE_RX_DMA==1)
/*! 
 *  @brief   	UART DMA接收函数
 *  @param  	接收数组指针，最大接收长度
 *  @return  	正常设置接收返回0 | 无法正常设置接收返回USART_FLAG_BSY
 *  @other  	正常发挥功能需要在接收DMA的中断函数、串口中断函数内调用Recieve_IRQ()函数
 */
uint16_t cUART::Recieve_DMA(uint8_t *pData, uint16_t length)
{
	/*正在执行接收，返回串口忙*/
	if(usart_flag_get(this->UART,USART_FLAG_RBNE))
	{return USART_FLAG_BSY;}
	
	/*长度设置*/
	this->Target_Length = (length>RX_BUF_LEN)?RX_BUF_LEN:length;
	this->pRX_Data = pData;
	/*关闭通道*/
	dma_channel_disable(this->DMAr,this->DMA_CHr);
	/*清除错误标志和IDLE标志*/
	usart_flag_get(this->UART,USART_FLAG_IDLE);
	usart_interrupt_flag_clear(this->UART,USART_INT_FLAG_IDLE);
	usart_data_receive(this->UART);
	/*开启空闲中断*/
	usart_interrupt_enable(this->UART,USART_INT_IDLE);
	/*配置本次传输*/
	dma_periph_increase_disable(this->DMAr,this->DMA_CHr);
	dma_periph_address_config(this->DMAr,this->DMA_CHr,(uint32_t)&USART_DATA(this->UART));
	dma_memory_address_config(this->DMAr,this->DMA_CHr,(uint32_t)this->pRX_BUF);
	dma_transfer_number_config(this->DMAr,this->DMA_CHr,RX_BUF_LEN);
	/*使能传输*/
	dma_channel_enable(this->DMAr,this->DMA_CHr);
	return 0;

}
#endif