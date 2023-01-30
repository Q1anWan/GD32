/*********************************************************************************
  *FileName:	UART_GD.h
  *Author:  	qianwan
  *Version:  	1.2
  *Date:  		2022/11/24
  *Description: GD32 ���ڷ��ͻ���
  *Other:		ʹ�ú궨�忪��DMA�շ���DMA�ڴ����
				//////////////////////////////////////////////////////////////////
			**	UART_USE_TX_DMA == 1ʱ��������DMA
				
				DMA������Ҫ�ڽ���DMA��DMAx_IRQHandler�����е���Transmit_IRQ
				//////////////////////////////////////////////////////////////////
			**	UART_USE_RX_DMA == 1ʱ��������DMA
				
				UART_TX_DMA_M2M == 0ʱΪ��ͨDMA����ģʽ
				��ͨ����ʹ��CPU�������ݣ��ʺ�С��168�ֽڵĽ��գ��Խ��Ͳ�������
				��ͨ������Ҫ����Ӧ���ڵ�USARTx_IRQHandler�����е���Recieve_IRQ
				
				UART_TX_DMA_M2M == 1ʱʹ���ڴ���˽���
				�ڴ����ʹ��DMA�������ݣ��ʺϴ���168�ֽڵĽ����Խ���CPUʹ����;�ڴ���˻�����Լ3us�����ݽ����ӳ�
				�ڴ���˽�����Ҫ����Ӧ���ڵ�USARTx_IRQHandler�����Լ�����DMA��DMAx_IRQHandler�����е���Recieve_IRQ
**********************************************************************************/
#ifndef UART_GD_H
#define UART_GD_H
#include <main.h>
#include <string.h>
#include "Delay.h"

/*******����DMA�շ�*****s***/
#define UART_USE_RX_DMA 1
#define UART_USE_TX_DMA 1
#define UART_TX_DMA_M2M 1
/********1��**0��**********/

/********��������**********/
//���ջ��������� Ҳ�������ܳ���
#define RX_BUF_LEN 1024
//ÿ��Byte�ĳ�ʱʱ��
#define UART_TIME_OVER_TIME 500
/**************************/
#ifdef __cplusplus

class cUART
{
	protected:
	uint32_t UART;
	
	public:
	uint16_t Target_Length=0;
	uint16_t Recieve_Length=0;
	
	uint16_t Transmit(uint8_t *DT,uint16_t num,uint32_t OVT);
	uint16_t Recieve(uint8_t *DT,uint16_t num,uint32_t OVT);

	virtual inline void Delay(void)
	{qDelay_us(1);}
	
	/*û��ʹ��DMA*/
	#if !(UART_USE_RX_DMA||UART_USE_TX_DMA)
	public:
	void UART_Init(uint32_t UART){this->UART = UART;usart_enable(this->UART);}
	#endif
	
	/*��ʹ��DMA����*/
	#if (UART_USE_TX_DMA==1)&&(UART_USE_RX_DMA==0)
	protected:
	uint32_t DMAt;
	dma_channel_enum DMA_CHt;
	public:
	void UART_Init(uint32_t UART,uint32_t DMAt,dma_channel_enum DMA_CHt)
	{
		this->UART = UART;
		this->DMAt = DMAt;
		this->DMA_CHt = DMA_CHt;
		usart_enable(this->UART);
	}

	/*��ʹ��DMA����*/
	#elif (UART_USE_TX_DMA==0)&&(UART_USE_RX_DMA==1)
	protected:
	uint32_t DMAr;
	dma_channel_enum DMA_CHr;
	uint8_t RX_BUF0[RX_BUF_LEN];
	uint8_t RX_BUF1[RX_BUF_LEN];
	uint8_t *pRX_Data;
	uint8_t *pRX_BUF = RX_BUF0;
	public:
	void UART_Init(uint32_t UART,uint32_t DMAr,dma_channel_enum DMA_CHr)
	{
		this->UART = UART;
		this->DMAr = DMAr;
		this->DMA_CHr = DMA_CHr;
		this->
		usart_enable(this->UART);
	}

	/*ʹ��DMA�շ�*/
	#elif (UART_USE_TX_DMA==1)&&(UART_USE_RX_DMA==1)
	protected:
	uint32_t DMAt;
	dma_channel_enum DMA_CHt;
	uint32_t DMAr;
	dma_channel_enum DMA_CHr;
	uint8_t RX_BUF0[RX_BUF_LEN];
	uint8_t RX_BUF1[RX_BUF_LEN];
	uint8_t *pRX_Data;
	uint8_t *pRX_BUF = RX_BUF0;
	public:
	void UART_Init(uint32_t UART,uint32_t DMAr,dma_channel_enum DMA_CHr,uint32_t DMAt,dma_channel_enum DMA_CHt)
	{
		this->UART = UART;
		this->DMAr = DMAr;
		this->DMA_CHr = DMA_CHr;
		this->DMAt = DMAt;
		this->DMA_CHt = DMA_CHt;
		usart_enable(this->UART);
	}
	#endif
	
	#if (UART_USE_TX_DMA==1)
	uint16_t Transmit_DMA(uint8_t *pData, uint16_t length);
	inline uint8_t Transmit_IRQ(void)
	{
		if(dma_interrupt_flag_get(this->DMAt,this->DMA_CHt,DMA_INT_FLAG_FTF))
		{
			dma_channel_disable(this->DMAt,this->DMA_CHt);
			dma_interrupt_flag_clear(this->DMAt,this->DMA_CHt,DMA_INT_FLAG_FTF);
			return 1;
		}
		return 0;
	}
	#endif
	#if (UART_USE_RX_DMA==1)
	uint16_t Recieve_DMA(uint8_t *pData, uint16_t length);
	inline uint8_t Recieve_IRQ(void)
	{ 
		//����������
		if(usart_interrupt_flag_get(this->UART,USART_INT_FLAG_RBNE_ORERR))
		{usart_interrupt_flag_clear(this->UART,USART_INT_FLAG_RBNE_ORERR);}
		if(usart_interrupt_flag_get(this->UART,USART_INT_FLAG_IDLE))
		{
			/*ֹͣDMA����*/
			dma_channel_disable(this->DMAr,this->DMA_CHr);
			/*�رտ����ж�*/
			usart_interrupt_disable(this->UART,USART_INT_IDLE);
			/*ͳ�ƴ�������*/
			this->Recieve_Length = RX_BUF_LEN - dma_transfer_number_get(this->DMAr,this->DMA_CHr);
			
			#if (UART_TX_DMA_M2M==1)		
			/*����M2M��DMA*/
			dma_periph_increase_enable(this->DMAr,this->DMA_CHr);
			dma_memory_to_memory_enable(this->DMAr,this->DMA_CHr);
			dma_periph_address_config(this->DMAr,this->DMA_CHr,(uint32_t)this->pRX_BUF);
			dma_memory_address_config(this->DMAr,this->DMA_CHr,(uint32_t)this->pRX_Data);
			dma_transfer_number_config(this->DMAr,this->DMA_CHr,(this->Recieve_Length>this->Target_Length)?this->Target_Length:this->Recieve_Length);//�������
			/*DMA�ж�ʹ��*/
			dma_interrupt_flag_clear(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF);
			dma_interrupt_enable(this->DMAr,this->DMA_CHr,DMA_INT_FTF);
			/*��ʼ����*/
			dma_channel_enable(this->DMAr,this->DMA_CHr);
			#else
			memcpy(this->pRX_Data,this->pRX_BUF,(this->Recieve_Length>this->Target_Length)?this->Target_Length:this->Recieve_Length);
			#endif

			/*˫����������*/
			this->pRX_BUF = (this->pRX_BUF==this->RX_BUF0)?this->RX_BUF1:this->RX_BUF0;
			/*���IDLE�жϱ�־*/
			usart_interrupt_flag_clear(this->UART,USART_INT_FLAG_IDLE);
			return 1;
		}
		#if (UART_TX_DMA_M2M==1)	
		else if(dma_interrupt_flag_get(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF))
		{
			/*ֹͣDMA����*/
			dma_channel_disable(this->DMAr,this->DMA_CHr);
			/*�ر�M2Mģʽ*/
			dma_memory_to_memory_disable(this->DMAr,this->DMA_CHr);
			dma_interrupt_disable(this->DMAr,this->DMA_CHr,DMA_INT_FTF);
			dma_interrupt_flag_clear(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF);
			return 2;
		}
		#endif
		return 0;
	}
	#endif
};
#endif

#endif