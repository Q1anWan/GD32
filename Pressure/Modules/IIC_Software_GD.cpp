/*********************************************************************************
  *FileName:	IIC_Software_GD.cpp
  *Author:  	qianwan
  *Version:  	1.6
  *Date:  		2022/11/20
  *Description: 软件IIC的基类，提供软件IIC的基本方法
  *Other:		软件IIC设计速率100Kbps，实际92Kbps
				IO电平读写使用内联函数,ACK仅提供方法
				GD32固件库支持
**********************************************************************************/
#include "IIC_Software_GD.h"

/*! 
 *  @brief      软件IIC初始化
 *  @param  	SCL SDA 引脚
 */
void cIIC::IIC_Init(uint32_t SCL_Port,uint32_t SCL_Pin,uint32_t SDA_Port,uint32_t SDA_Pin)
{
	//初始化数据指针
	this->SCL_Port = SCL_Port;
	this->SCL_Pin  = SCL_Pin;
	this->SDA_Port = SDA_Port;
	this->SDA_Pin  = SDA_Pin;
	//拉高引脚引入空闲状态
	this->SCL_1();
	this->SDA_1();
}

/*! 
 *  @brief      执行IIC开始传输电平动作
 *  @brief  	此函数不需要IIC_Stop函数前置，可以用作Restart
 */
void cIIC::IIC_Start(void)
{
	//拉低时钟，SDA置高准备下降沿
	this->SCL_0();
	this->Delay(2);
	this->SDA_1();
	this->Delay(3);
	//下一个时钟来临
	this->SCL_1();
	this->Delay(5);
	//发生下降沿
	this->SDA_0();
	this->Delay(5);
	//拉低时钟完成START
	this->SCL_0();
}

/*! 
 *  @brief      执行IIC结束传输电平动作
 *  @brief  	总线将进入空闲状态
 */
void cIIC::IIC_Stop(void)
{
	//拉低SDA准备上升沿
	this->SCL_0();
	this->Delay(2);
	this->SDA_0();
	this->Delay(3);
	//拉高SCL
	this->SCL_1();
	this->Delay(5);
	//SDA产生上升沿
	this->SDA_1();
	//总线进入空闲状态
	this->Delay(5);
}
/*! 
 *  @brief      执行ACK电平动作
 */
void cIIC::IIC_ACK(void)
{
	//拉低时钟
	this->SCL_0();
	this->Delay(2);
	//SDA拉低，ACK信号
	this->SDA_0();
	this->Delay(3);
	//拉高时钟
	this->SCL_1();
	this->Delay(5);
	//拉低时钟与SDA
	this->SCL_0();
	this->SDA_0();
}

/*! 
 *  @brief      执行NACK电平动作
 */
void cIIC::IIC_NACK(void)
{
	//拉低时钟
	this->SCL_0();
	this->Delay(2);
	//SDA拉高，NACK信号
	this->SDA_1();
	this->Delay(3);
	//拉高时钟
	this->SCL_1();
	this->Delay(5);
	//拉低时钟与SDA
	this->SCL_0();
	this->SDA_0();
}

/*! 
 *  @brief      从设备ACK
 *  @brief  	返回从设备ACK状态
 *  @brief  	返回1:ACK 返回0:NACK
 */
uint8_t cIIC::IIC_Wait_ACK(void)
{
	uint8_t ACK=0;
	this->SCL_0();
	this->Delay(2);
	this->SDA_1();
	this->Delay(3);
	this->SCL_1();
	this->Delay(2);
	ACK = this->SDA_Read()?0:1;
	this->Delay(3);
	this->SCL_0();
	this->SDA_0();
	return ACK;	
}

/*! 
 *  @brief      IIC数据发送
 *  @param  	Data:待发送的数据
 *  @return  	返回从设备ACK状态
 */
uint8_t cIIC::IIC_SendOneByte(uint8_t Data)
{
	for(uint8_t i=0;i<8;i++)
	{
		this->SCL_0();
		this->Delay(2);
		(Data & 0x80)?this->SDA_1():this->SDA_0();
		Data <<= 1;
		this->Delay(3);
		this->SCL_1();
		this->Delay(5);
	}
	return this->IIC_Wait_ACK();
}

/*! 
 *  @brief      IIC数据读取
 *  @return  	返回接收到的1字节数据
 *	@brief		此函数不提供ACK,ACK操作需要在派生类的方式钟实现
 */
uint8_t cIIC::IIC_ReadOneByte(void)
{
	uint8_t rec=0;
	this->SCL_0();
	this->SDA_1();
	for(uint8_t i=0;i<8;i++)
	{

		this->Delay(5);
		this->SCL_1();
		this->Delay(3);
		rec<<=1;
		rec += this->SDA_Read();
		this->Delay(2);
		this->SCL_0();
	}
	this->SDA_0();
	return 	rec;
}
