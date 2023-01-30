/*********************************************************************************
  *FileName:	IIC_Software_GD.cpp
  *Author:  	qianwan
  *Version:  	1.6
  *Date:  		2022/11/20
  *Description: ���IIC�Ļ��࣬�ṩ���IIC�Ļ�������
  *Other:		���IIC�������100Kbps��ʵ��92Kbps
				IO��ƽ��дʹ����������,ACK���ṩ����
				GD32�̼���֧��
**********************************************************************************/
#include "IIC_Software_GD.h"

/*! 
 *  @brief      ���IIC��ʼ��
 *  @param  	SCL SDA ����
 */
void cIIC::IIC_Init(uint32_t SCL_Port,uint32_t SCL_Pin,uint32_t SDA_Port,uint32_t SDA_Pin)
{
	//��ʼ������ָ��
	this->SCL_Port = SCL_Port;
	this->SCL_Pin  = SCL_Pin;
	this->SDA_Port = SDA_Port;
	this->SDA_Pin  = SDA_Pin;
	//���������������״̬
	this->SCL_1();
	this->SDA_1();
}

/*! 
 *  @brief      ִ��IIC��ʼ�����ƽ����
 *  @brief  	�˺�������ҪIIC_Stop����ǰ�ã���������Restart
 */
void cIIC::IIC_Start(void)
{
	//����ʱ�ӣ�SDA�ø�׼���½���
	this->SCL_0();
	this->Delay(2);
	this->SDA_1();
	this->Delay(3);
	//��һ��ʱ������
	this->SCL_1();
	this->Delay(5);
	//�����½���
	this->SDA_0();
	this->Delay(5);
	//����ʱ�����START
	this->SCL_0();
}

/*! 
 *  @brief      ִ��IIC���������ƽ����
 *  @brief  	���߽��������״̬
 */
void cIIC::IIC_Stop(void)
{
	//����SDA׼��������
	this->SCL_0();
	this->Delay(2);
	this->SDA_0();
	this->Delay(3);
	//����SCL
	this->SCL_1();
	this->Delay(5);
	//SDA����������
	this->SDA_1();
	//���߽������״̬
	this->Delay(5);
}
/*! 
 *  @brief      ִ��ACK��ƽ����
 */
void cIIC::IIC_ACK(void)
{
	//����ʱ��
	this->SCL_0();
	this->Delay(2);
	//SDA���ͣ�ACK�ź�
	this->SDA_0();
	this->Delay(3);
	//����ʱ��
	this->SCL_1();
	this->Delay(5);
	//����ʱ����SDA
	this->SCL_0();
	this->SDA_0();
}

/*! 
 *  @brief      ִ��NACK��ƽ����
 */
void cIIC::IIC_NACK(void)
{
	//����ʱ��
	this->SCL_0();
	this->Delay(2);
	//SDA���ߣ�NACK�ź�
	this->SDA_1();
	this->Delay(3);
	//����ʱ��
	this->SCL_1();
	this->Delay(5);
	//����ʱ����SDA
	this->SCL_0();
	this->SDA_0();
}

/*! 
 *  @brief      ���豸ACK
 *  @brief  	���ش��豸ACK״̬
 *  @brief  	����1:ACK ����0:NACK
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
 *  @brief      IIC���ݷ���
 *  @param  	Data:�����͵�����
 *  @return  	���ش��豸ACK״̬
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
 *  @brief      IIC���ݶ�ȡ
 *  @return  	���ؽ��յ���1�ֽ�����
 *	@brief		�˺������ṩACK,ACK������Ҫ��������ķ�ʽ��ʵ��
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
