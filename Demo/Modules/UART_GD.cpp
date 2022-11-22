/*********************************************************************************
  *FileName:	UART_GD.cpp
  *Author:  	qianwan
  *Version:  	1.0
  *Date:  		2022/11/21
  *Description: GD32 ���ڷ��ͻ���
  *Other:		ʹ�ú궨�忪��DMA�շ�
  *Other:		ʹ��DMA����ʱ����������ڷ���DMA���жϺ����ڵ���Transmit_IRQ����
  *Other:		ʹ��DMA����ʱ����������ڽ���DMA���жϺ����������жϺ����ڵ���Recieve_IRQ()����
**********************************************************************************/
#include "UART_GD.h"
#include "string.h"


/*! 
 *  @brief      �����������ͺ���
 *  @param  	���뷢������ָ�롢������Ŀ
  *  @return  	������������0 | �����쳣����USART_FLAG_BSY
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
 *  @brief      �����������ͺ���
 *  @param  	���뷢������ָ�롢������Ŀ����ʱus
 *  @return  	������������0 | �����쳣����USART_FLAG_RT
 */
uint16_t cUART::Recieve(uint8_t *DT,uint16_t num,uint32_t OVT)
{
	uint16_t wait_cnt = 0;
	uint16_t i = 0;

	/*����ѭ������*/
	for (; i < num; i++)
	{
		/*�ȴ����ݲ���¼��ʱ*/
		while(!(USART_REG_VAL(this->UART, USART_FLAG_RBNE) & BIT(USART_BIT_POS(USART_FLAG_RBNE))))
		{ 
			this->Delay(1);
			if(wait_cnt++>OVT)
			{
				this->Recieve_Length = i;
				return USART_FLAG_RT;
			}
		}
		
		/*RBNEʱ�������ݣ��˲��������RBNE��־*/
		DT[i]=(uint16_t)(GET_BITS(USART_DATA(this->UART), 0U, 8U));
		wait_cnt = 0;
	}
	
	this->Recieve_Length = i;
	return 0;
}
#if (UART_USE_TX_DMA==1)
/*! 
 *  @brief   	UART DMA���ͺ���
 *  @param  	��������ָ�룬�����ֽڳ���
 *  @return  	������������0 | �����쳣����USART_FLAG_BSY
 *  @other  	�������ӹ�����Ҫ�ڷ���DMA���жϺ����ڵ���Transmit_IRQ����
 */
uint16_t cUART::Transmit_DMA(uint8_t *pData, uint16_t length)
{
	/*����ִ�з��ͣ����ش���æ*/
	if(!usart_flag_get(this->UART,USART_FLAG_TBE))
	{return USART_FLAG_BSY;}

	/*���ñ��δ���*/
	dma_interrupt_flag_clear(this->DMAt,this->DMA_CHt,DMA_INT_FLAG_FTF);
	dma_memory_address_config(this->DMAt,this->DMA_CHt,(uint32_t)pData);
	dma_transfer_number_config(this->DMAt,this->DMA_CHt,(uint32_t)length);
	/*ʹ�ܴ���*/
	dma_channel_enable(this->DMAt,this->DMA_CHt);
	return 0;
}
#endif

#if (UART_USE_RX_DMA==1)
/*! 
 *  @brief   	UART DMA���պ���
 *  @param  	��������ָ�룬�����ճ���
 *  @return  	�������ý��շ���0 | �޷��������ý��շ���USART_FLAG_BSY
 *  @other  	�������ӹ�����Ҫ�ڽ���DMA���жϺ����������жϺ����ڵ���Recieve_IRQ()����
 */
uint16_t cUART::Recieve_DMA(uint8_t *pData, uint16_t length)
{
	/*����ִ�н��գ����ش���æ*/
	if(usart_flag_get(this->UART,USART_FLAG_RBNE))
	{return USART_FLAG_BSY;}
	
	/*��������*/
	this->Target_Length = (length>RX_BUF_LEN)?RX_BUF_LEN:length;
	this->pRX_Data = pData;
	/*�ر�ͨ��*/
	dma_channel_disable(this->DMAr,this->DMA_CHr);
	/*��������־��IDLE��־*/
	usart_flag_get(this->UART,USART_FLAG_IDLE);
	usart_interrupt_flag_clear(this->UART,USART_INT_FLAG_IDLE);
	usart_data_receive(this->UART);
	/*���������ж�*/
	usart_interrupt_enable(this->UART,USART_INT_IDLE);
	/*���ñ��δ���*/
	dma_periph_increase_disable(this->DMAr,this->DMA_CHr);
	dma_periph_address_config(this->DMAr,this->DMA_CHr,(uint32_t)&USART_DATA(this->UART));
	dma_memory_address_config(this->DMAr,this->DMA_CHr,(uint32_t)this->pRX_BUF);
	dma_transfer_number_config(this->DMAr,this->DMA_CHr,RX_BUF_LEN);
	/*ʹ�ܴ���*/
	dma_channel_enable(this->DMAr,this->DMA_CHr);
	return 0;

}
#endif