/*********************************************************************************
  *FileName:	SPI_Software.h
  *Author:  	qianwan
  *Version:  	1.4
  *Date:  		2022/10/19
  *Description: ���SPI�Ļ��࣬�ṩ���SPI�Ļ�������
  *Other:		IO��ƽ��дʹ����������
**********************************************************************************/
#ifndef SPI_SOFTWARE_H
#define SPI_SOFTWARE_H
#include <main.h>
#include "Delay.h"
#ifdef __cplusplus
/*! 
 *  @brief      ���SPI����
 *  @brief		Ϊ�����ṩSPI��ʼ�������ݽ���������CS���ơ�MISO���ƵȻ�������
 */
class cSPI
{
	protected:
	uint32_t MOSI_Port;
	uint32_t MOSI_Pin;
	uint32_t MISO_Port;
	uint32_t MISO_Pin;
	uint32_t SCL_Port;
	uint32_t SCL_Pin;
	uint32_t CS_Port;
	uint32_t CS_Pin;
	
	inline void Delay(uint16_t us);
	inline void MOSI_1(void);
	inline void MOSI_0(void);
	inline void SCL_1(void);
	inline void SCL_0(void);
	inline uint8_t MISO_Read(void);
	
	public:
	void SPI_Init(	uint32_t MOSI_Port,uint32_t MOSI_Pin,
					uint32_t MISO_Port,uint32_t MISO_Pin,
					uint32_t SCL_Port ,uint32_t SCL_Pin,
					uint32_t CS_Port  ,uint32_t CS_Pin);
	inline void CS_0(void);
	inline void CS_1(void);
	inline void MISO_OD(void);
	inline void MISO_PP(void);
	uint8_t SPI_ExchangeOneByte(uint8_t Data);
};
/*! 
 *  @brief      SPI����΢����ʱ����
 *  @param  	��ʱʱ��(ʵ��ƫ��)
 */
inline void cSPI::Delay(uint16_t us)
{
	qDelay_us(us);
//	__NOP();
//	__NOP();
//	__NOP();
//	__NOP();
//	__NOP();
//	__NOP();
}
/*! 
 *  @brief      ����ĺ������ڿ��Ƹ����ŵ�ƽ 
 */
inline void cSPI::MOSI_1(void)
{GPIO_BOP(this->MOSI_Port)=this->MOSI_Pin;}
inline void cSPI::MOSI_0(void)
{GPIO_BC(this->MOSI_Port)=this->MOSI_Pin;}
inline uint8_t cSPI::MISO_Read(void)
{return (GPIO_ISTAT(this->MISO_Port)&(this->MISO_Pin))?1:0;}
inline void cSPI::SCL_1(void)
{GPIO_BOP(this->SCL_Port)=this->SCL_Pin;}
inline void cSPI::SCL_0(void)
{GPIO_BC(this->SCL_Port)=this->SCL_Pin;}
inline void cSPI::CS_0(void)
{GPIO_BC(this->CS_Port)=this->CS_Pin;}
inline void cSPI::CS_1(void)
{GPIO_BOP(this->CS_Port)=this->CS_Pin;}
/*! 	
 *  @brief      ����ĺ���������MOSI��MISO����ʱ���������������
 */
inline void cSPI::MISO_OD(void)//��©
{gpio_init(this->MISO_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX,this->MISO_Pin);}
inline void cSPI::MISO_PP(void)//����
{gpio_init(this->MISO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,this->MISO_Pin);}

#endif
#endif