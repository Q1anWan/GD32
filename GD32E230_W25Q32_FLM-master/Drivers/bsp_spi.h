#ifndef __BSP_SPI_H
#define __BSP_SPI_H

#include "gd32e23x.h"

/* SPI0 外设初始化 */
void spi0_init(void);

/* SPI0读写的一字节数据 */
uint8_t spi0_rw_byte(uint8_t byte);

/* SPI0 CS电平设置 */
void spi0_cs_set(int value);
    
#endif /* __BSP_SPI_H */

/* end of file */
