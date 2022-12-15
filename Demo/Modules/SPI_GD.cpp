/*********************************************************************************
  *FileName:	SPI_GD.cpp
  *Author:  	qianwan
  *Version:  	1.1
  *Date:  		2022/11/30
  *Description: GD32 SPI����
  *Other:		ʹ�ú궨�忪��DMA�շ�
				//////////////////////////////////////////////////////////////////
			**	SPI_USE_TX_DMA == 1ʱ��������DMA
				
				DMA������Ҫ�ڷ���DMA��DMAx_IRQHandler�����е���IRQ_Tx
				//////////////////////////////////////////////////////////////////
			**	SPI_USE_RX_DMA == 1ʱ��������DMA
				
				DMA������Ҫ�ڽ���DMA��DMAx_IRQHandler�����е���IRQ_Rx
**********************************************************************************/
#include "SPI_GD.h"

uint8_t cSPI::SPI_ExchangeOneByte(uint8_t Data)
{
	uint16_t wait_cnt = 0;
	while(!spi_i2s_flag_get(this->SPI,SPI_FLAG_TBE))
	{
		if (wait_cnt++ > SPI_TIME_OVER_TIME)
		{return 0xff;}
		this->Delay();
	}
	spi_i2s_data_transmit(this->SPI,Data);
	while(!spi_i2s_flag_get(this->SPI,I2S_FLAG_RBNE))
	{
		if (wait_cnt++ > SPI_TIME_OVER_TIME)
		{return 0xff;}
		this->Delay();
	}
	return spi_i2s_data_receive(this->SPI);
}

#if (SPI_USE_TX_DMA==1)
uint16_t cSPI::Transmit_DMA(uint8_t *data,uint16_t num)
{
	/*����ִ�з��ͻ���գ�����SPIæ*/
	if(spi_i2s_flag_get(this->SPI,SPI_FLAG_TRANS))
	{return SPI_FLAG_TRANS;}

	/*���ñ��δ���*/
	dma_interrupt_flag_clear(this->DMAt,this->DMA_CHt,DMA_INT_FLAG_FTF);
	dma_memory_address_config(this->DMAt,this->DMA_CHt,(uint32_t)data);
	dma_transfer_number_config(this->DMAt,this->DMA_CHt,(uint32_t)num);
	/*ʹ�ܴ���*/
	dma_channel_enable(this->DMAt,this->DMA_CHt);
	return 0;
}
uint8_t cSPI::IRQ_Tx(void)
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
#if (SPI_USE_RX_DMA==1)
uint16_t cSPI::Receive_DMA(uint8_t *data,uint16_t num)
{
	/*����ִ�з��ͻ���գ�����SPIæ*/
	if(spi_i2s_flag_get(this->SPI,SPI_FLAG_TRANS))
	{return SPI_FLAG_TRANS;}
	
	/*���ñ��δ���*/
	dma_interrupt_flag_clear(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF);
	dma_memory_address_config(this->DMAr,this->DMA_CHr,(uint32_t)data);
	dma_transfer_number_config(this->DMAr,this->DMA_CHr,(uint32_t)num);
	/*ʹ�ܴ���*/
	dma_channel_enable(this->DMAr,this->DMA_CHr);		
	return 0;
}
uint8_t cSPI::IRQ_Rx(void)
{
	if(dma_interrupt_flag_get(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF))
	{
		dma_channel_disable(this->DMAr,this->DMA_CHr);
		dma_interrupt_flag_clear(this->DMAr,this->DMA_CHr,DMA_INT_FLAG_FTF);
		return 1;
	}
	return 0;	
}
#endif

