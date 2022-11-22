/*********************************************************************************
  *FileName:	IIC_Software_GD.h
  *Author:  	qianwan
  *Version:  	1.6
  *Date:  		2022/11/20
  *Description: ���IIC�Ļ��࣬�ṩ���IIC�Ļ�������
  *Other:		���IIC�������100Kbps��ʵ��92Kbps
				IO��ƽ��дʹ����������,ACK���ṩ����
				GD32�̼���֧��
**********************************************************************************/
#ifndef IIC_SOFTWARE_H
#define IIC_SOFTWARE_H
#include <main.h>
#include "Delay.h"
#ifdef __cplusplus
/*! 
 *  @brief      ���IIC����
 *  @brief		Ϊ�����ṩIIC��ʼ���������շ�������ACK���ƺ�����IIC��ʼ/ֹͣ�����Ȼ�������
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
 *  @brief      IIC����΢����ʱ����
 *  @param  	��ʱʱ��(ʵ��ƫ��)
 */
inline void cIIC::Delay(uint16_t us)
{qDelay_us(us);}

/*! 
 *  @brief      ����ĺ������ڿ��Ƹ����ŵ�ƽ 
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
 *  @brief      ��ȡSDA���ŵ�ƽ
 *  @return     SDA���ŵ�ƽ
 */
inline uint8_t cIIC::SDA_Read(void)
{return (GPIO_ISTAT(this->SDA_Port)&(this->SDA_Pin))?1:0;}

#endif
#endif
