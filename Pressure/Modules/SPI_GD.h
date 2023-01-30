/*********************************************************************************
  *FileName:	SPI_GD.h
  *Author:  	qianwan
  *Version:  	1.2
  *Date:  		2023/01/29
  *Description: GD32 SPI控制
  *Other:		使用宏定义开启DMA收发
				//////////////////////////////////////////////////////////////////
			**	SPI_USE_TX_DMA == 1时开启发送DMA
				
				DMA发送需要在发送DMA的DMAx_IRQHandler函数中调用IRQ_Tx
				//////////////////////////////////////////////////////////////////
			**	SPI_USE_RX_DMA == 1时开启接收DMA
				
				DMA接收需要在接收DMA的DMAx_IRQHandler函数中调用IRQ_Rx
**********************************************************************************/
#ifndef SPI_GD_H
#define SPI_GD_H
#include <main.h>
#include "Delay.h"
#ifdef __cplusplus
/*******开启DMA收发********/
#define SPI_USE_TX_DMA 1
#define SPI_USE_RX_DMA 1
/**************************/
#define SPI_TIME_OVER_TIME 500
/*! 
 *  @brief      软件SPI基类
 *  @brief		为外设提供SPI初始化、数据交换函数、CS控制、MISO控制等基本方法
 */
class cSPI
{
	protected:
	uint32_t SPI;
	uint32_t CS_Port;
	uint32_t CS_Pin;
	virtual inline void Delay(void){__NOP();__NOP();__NOP();}

	public:
	inline void CS_0(void)
	{GPIO_BC(this->CS_Port)=this->CS_Pin;}
	inline void CS_1(void)
	{GPIO_BOP(this->CS_Port)=this->CS_Pin;}

	uint8_t SPI_ExchangeOneByte(uint8_t Data);
	
	#if !(SPI_USE_RX_DMA||SPI_USE_TX_DMA)
	public:
	void SPI_Init(uint32_t SPI, uint32_t CS_Port, uint32_t CS_Pin)
	{
		this->SPI = SPI;
		this->CS_Port = CS_Port;
		this->CS_Pin = CS_Pin;
		spi_enable(this->SPI);
		this->CS_1();
	}
	#endif
	
	#if (SPI_USE_TX_DMA==1)&&(SPI_USE_RX_DMA==0)
	protected:
	uint32_t DMAt;
	dma_channel_enum DMA_CHt;
	public:
	void SPI_Init(uint32_t SPI, uint32_t CS_Port, uint32_t CS_Pin,
				  uint32_t DMA,dma_channel_enum DMA_CH)
	{
		this->SPI = SPI;
		this->CS_Port = CS_Port;
		this->CS_Pin = CS_Pin;
		this->DMAt = DMA;
		this->DMA_CHt = DMA_CH;
		spi_enable(this->SPI);
		this->CS_1();
		
	}
	uint8_t Transmit_DMA(uint8_t *data,uint16_t num);
	uint8_t IRQ_Tx(void);
	
	/*仅使用DMA接收*/
	#elif (SPI_USE_TX_DMA==0)&&(SPI_USE_RX_DMA==1)
	protected:
	uint32_t DMAr;
	dma_channel_enum DMA_CHr;
	public:
	void SPI_Init(uint32_t SPI, uint32_t CS_Port, uint32_t CS_Pin,
				  uint32_t DMA,dma_channel_enum DMA_CH)
	{
		this->SPI = SPI;
		this->CS_Port = CS_Port;
		this->CS_Pin = CS_Pin;
		this->DMAr = DMA;
		this->DMA_CHr = DMA_CH;
		spi_enable(this->SPI);
		this->CS_1();
	}
	uint8_t Receive_DMA(uint8_t *data,uint16_t num);
	uint8_t IRQ_Rx(void);
	
	/*使用DMA收发*/
	#elif (SPI_USE_TX_DMA==1)&&(SPI_USE_RX_DMA==1)
	protected:
	uint32_t DMAt;
	dma_channel_enum DMA_CHt;
	uint32_t DMAr;
	dma_channel_enum DMA_CHr;
	public:
	void SPI_Init(uint32_t SPI, uint32_t CS_Port, uint32_t CS_Pin, 
				  uint32_t DMAr, dma_channel_enum DMA_CHr, 
				  uint32_t DMAt,dma_channel_enum DMA_CHt)
	{
		this->SPI = SPI;
		this->CS_Port = CS_Port;
		this->CS_Pin = CS_Pin;
		this->DMAt = DMAt;
		this->DMA_CHt = DMA_CHt;
		this->DMAr = DMAr;
		this->DMA_CHr = DMA_CHr;
		spi_enable(this->SPI);
		this->CS_1();
	}
	
	uint16_t Transmit_DMA(uint8_t *data,uint16_t num);
	uint16_t Receive_DMA(uint8_t *data,uint16_t num);
	uint8_t IRQ_Rx(void);
	uint8_t IRQ_Tx(void);
	#endif
};


#endif
#endif