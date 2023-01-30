/*********************************************************************************
  *FileName:	SPI_Software_GD.cpp
  *Author:  	qianwan
  *Version:  	1.4
  *Date:  		2022/11/21
  *Description: ���SPI�Ļ��࣬�ṩ���SPI�Ļ�������
  *Other:		IO��ƽ��дʹ����������
**********************************************************************************/
#include "SPI_Software_GD.h"
/*! 
 *  @brief      ���SPI��ʼ��
 *  @param  	MOSI MISO SCL CS ����
 *  @param  	������SPI�£�MOSI��MISO������DT����ͨ���л����ΪPP��ODʵ�ְ�˫��
 */
void cSPI::SPI_Init(uint32_t MOSI_Port,uint32_t MOSI_Pin,
					uint32_t MISO_Port,uint32_t MISO_Pin,
					uint32_t SCL_Port ,uint32_t SCL_Pin,
					uint32_t CS_Port  ,uint32_t CS_Pin)
{
	this->MOSI_Port = MOSI_Port;
	this->MOSI_Pin  = MOSI_Pin;
	this->MISO_Port = MISO_Port;
	this->MISO_Pin	= MISO_Pin;
	this->SCL_Port	= SCL_Port;
	this->SCL_Pin	= SCL_Pin;
	this->CS_Port	= CS_Port;
	this->CS_Pin	= CS_Pin; 
	
	this->MOSI_0();
	this->SCL_0();
}
/*! 
 *  @brief  	SPI���ݽ���
 *  @param  	CPOL:0 CPHA:0
 */
uint8_t cSPI::SPI_ExchangeOneByte(uint8_t Data)
{
	uint8_t rec=0;
	for(uint8_t i=0;i<8;i++)
	{
		(Data&0x80)?this->MOSI_1():this->MOSI_0();
		this->Delay(1);
		Data<<=1;
		this->SCL_1();
		this->Delay(1);
		rec<<=1;
		rec += MISO_Read();
		this->SCL_0();
	}
	this->MOSI_0();
	return rec;
}