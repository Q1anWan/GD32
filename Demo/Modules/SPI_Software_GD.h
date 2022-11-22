/*********************************************************************************
  *FileName:	SPI_Software.h
  *Author:  	qianwan
  *Version:  	1.4
  *Date:  		2022/10/19
  *Description: 软件SPI的基类，提供软件SPI的基本方法
  *Other:		IO电平读写使用内联函数
**********************************************************************************/
#ifndef SPI_SOFTWARE_H
#define SPI_SOFTWARE_H
#include <main.h>
#include "Delay.h"
#ifdef __cplusplus
/*! 
 *  @brief      软件SPI基类
 *  @brief		为外设提供SPI初始化、数据交换函数、CS控制、MISO控制等基本方法
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
 *  @brief      SPI基类微秒延时函数
 *  @param  	延时时间(实际偏大)
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
 *  @brief      下面的函数用于控制各引脚电平 
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
 *  @brief      下面的函数用于在MOSI和MISO共线时调整引脚输出类型
 */
inline void cSPI::MISO_OD(void)//开漏
{gpio_init(this->MISO_Port, GPIO_MODE_OUT_OD, GPIO_OSPEED_MAX,this->MISO_Pin);}
inline void cSPI::MISO_PP(void)//推挽
{gpio_init(this->MISO_Port, GPIO_MODE_OUT_PP, GPIO_OSPEED_MAX,this->MISO_Pin);}

#endif
#endif