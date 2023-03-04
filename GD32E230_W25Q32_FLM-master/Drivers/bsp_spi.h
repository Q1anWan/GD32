#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "gd32e23x.h"

/* SPI0 �����ʼ�� */
void spi0_init(void);

/* SPI0��д��һ�ֽ����� */
uint8_t spi0_rw_byte(uint8_t byte);

/* SPI0 CS��ƽ���� */
void spi0_cs_set(int value);
    
#endif /* __BSP_SPI_H */

/* end of file */
