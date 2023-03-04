#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H

#include "gd32e23x.h"

/* FLASH ID定义 */
#define W25Q80                  0XEF13
#define W25Q16                  0XEF14
#define W25Q32                  0XEF15
#define W25Q64                  0XEF16

/* 指令列表 */
#define W25X_WriteEnable        0x06
#define W25X_WriteDisable       0x04
#define W25X_ReadStatusReg      0x05
#define W25X_WriteStatusReg     0x01
#define W25X_ReadData           0x03
#define W25X_FastReadData       0x0B
#define W25X_FastReadDual       0x3B
#define W25X_PageProgram        0x02
#define W25X_BlockErase         0xD8
#define W25X_SectorErase        0x20
#define W25X_ChipErase          0xC7
#define W25X_PowerDown          0xB9
#define W25X_ReleasePowerDown   0xAB
#define W25X_DeviceID           0xAB
#define W25X_ManufactDeviceID   0x90
#define W25X_JedecDeviceID      0x9F

//读取芯片ID
uint16_t spi_flash_read_id(void);

//擦除一个扇区
void spi_flash_erase_sector(uint32_t dst_addr);

//擦除整个芯片
void spi_flash_erase_chip(void);

//无检验写SPI FLASH 
void spi_flash_write_no_check(uint32_t addr, uint8_t *p_buf, uint16_t bytes);

//SPI在一页(0~65535)内写入少于256个字节的数据
void spi_flash_write_page(uint32_t addr, uint8_t *p_buf, uint16_t bytes);

//指定地址读取SPI FLASH一字节  
uint8_t spi_flash_read_byte(uint32_t addr);

//指定地址读取SPI FLASH
void spi_flash_read(uint32_t addr, uint8_t *p_buf, uint16_t bytes);

#endif /* __SPI_FLASH_H */

/* end of file */
