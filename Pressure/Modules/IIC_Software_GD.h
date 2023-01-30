/*********************************************************************************
  *FileName:	IIC_Software_GD.h
  *Author:  	qianwan
  *Version:  	1.6
  *Date:  		2022/11/20
  *Description: 软件IIC的基类，提供软件IIC的基本方法
  *Other:		软件IIC设计速率100Kbps，实际92Kbps
				IO电平读写使用内联函数,ACK仅提供方法
				GD32固件库支持
**********************************************************************************/
#ifndef IIC_SOFTWARE_H
#define IIC_SOFTWARE_H
#include <main.h>
#include "Delay.h"
#ifdef __cplusplus
/*! 
 *  @brief      软件IIC基类
 *  @brief		为外设提供IIC初始化、数据收发函数、ACK控制函数、IIC开始/停止函数等基本方法
 */
class cIIC
{
	protected:
	uint32_t SCL_Port;
	uint32_t SCL_Pin;
	uint32_t SDA_Port;
	uint32_t SDA_Pin;
	
	inline void Delay(uint16_t us);
	inline void SDA_0(void);
	inline void SDA_1(void);
	inline void SCL_0(void);
	inline void SCL_1(void);
	
	inline uint8_t SDA_Read(void);
	
	public:
	void IIC_Init(	uint32_t SCL_Port,uint32_t SCL_Pin,
					uint32_t SDA_Port,uint32_t SDA_Pin);
	void IIC_Start(void);
	void IIC_Stop(void);
	void IIC_ACK(void);
	void IIC_NACK(void);
	uint8_t IIC_Wait_ACK(void);
	uint8_t IIC_SendOneByte(uint8_t Data);
	uint8_t IIC_ReadOneByte(void);
};
/*! 
 *  @brief      IIC基类微秒延时函数
 *  @param  	延时时间(实际偏大)
 */
inline void cIIC::Delay(uint16_t us)
{qDelay_us(us);}

/*! 
 *  @brief      下面的函数用于控制各引脚电平 
 */
inline void cIIC::SCL_1(void)
{GPIO_BOP(this->SCL_Port)=this->SCL_Pin;}
inline void cIIC::SDA_1(void)
{GPIO_BOP(this->SDA_Port)=this->SDA_Pin;}
inline void cIIC::SCL_0(void)
{GPIO_BC(this->SCL_Port)=this->SCL_Pin;}
inline void cIIC::SDA_0(void)
{GPIO_BC(this->SDA_Port)=this->SDA_Pin;}
/*! 
 *  @brief      读取SDA引脚电平
 *  @return     SDA引脚电平
 */
inline uint8_t cIIC::SDA_Read(void)
{return (GPIO_ISTAT(this->SDA_Port)&(this->SDA_Pin))?1:0;}

#endif
#endif
