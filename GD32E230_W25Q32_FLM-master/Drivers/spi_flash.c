#include "spi_flash.h" 
#include "bsp_spi.h"
#include "string.h"

/* SPI Flash CS */
#define SPI_FLASH_CS_LOW     spi0_cs_set(0)
#define SPI_FLASH_CS_HIGH    spi0_cs_set(1)

/* SPI写操作接口适配 */
static inline uint8_t spi_dev_transmit(uint8_t byte)
{
    return spi0_rw_byte(byte);
}

/* SPI读操作接口适配 */
static inline uint8_t spi_dev_receive(void)
{
    return spi0_rw_byte(0xFF);
}

//读取SPI_FLASH的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
static uint8_t spi_flash_read_state(void) 
{
    uint8_t byte = 0;

    SPI_FLASH_CS_LOW;                      /* 使能器件 */
    spi_dev_transmit(W25X_ReadStatusReg);  /* 发送读取状态寄存器命令 */
    byte = spi_dev_receive();          	   /* 读取一个字节 */
    SPI_FLASH_CS_HIGH;                     /* 取消片选 */

    return byte;
}

//SPI_FLASH写使能
//将WEL置位   
static void spi_flash_write_enable(void)   
{
    SPI_FLASH_CS_LOW;                      /* 使能器件 */
    spi_dev_transmit(W25X_WriteEnable);    /* 发送写使能 */
    SPI_FLASH_CS_HIGH;                     /* 取消片选 */
}

//等待空闲
static void spi_flash_wait_busy(void)
{
    while ((spi_flash_read_state() & 0x01) == 0x01);   /* 等待BUSY位清空 */
}

//读取芯片ID W25X16的ID:0XEF14
uint16_t spi_flash_read_id(void)
{
    uint16_t id = 0;

    SPI_FLASH_CS_LOW;
    spi_dev_transmit(0x90);  /* 送读取ID命令 */
    spi_dev_transmit(0x00); 
    spi_dev_transmit(0x00); 
    spi_dev_transmit(0x00);
    id |= spi_dev_receive() << 8;
    id |= spi_dev_receive();
    SPI_FLASH_CS_HIGH;

    return id;
}

//指定地址读取SPI FLASH
//在指定地址开始读取指定长度的数据
//addr:开始读取的地址(24bit)
//p_buf:数据存储区s
//NumByteToRead:要读取的字节数(最大65535)
void spi_flash_read(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{ 
     uint16_t i;

    SPI_FLASH_CS_LOW;                               /* 使能器件 */
    spi_dev_transmit(W25X_ReadData);                /* 发送读取命令 */
    spi_dev_transmit((uint8_t)((addr) >> 16));      /* 发送24bit地址 */
    spi_dev_transmit((uint8_t)((addr) >> 8));   
    spi_dev_transmit((uint8_t)addr);

    for (i = 0; i < bytes; i++) { 
        p_buf[i] = spi_dev_receive();  
    }

    SPI_FLASH_CS_HIGH;                              /* 取消片选 */
}

uint8_t spi_flash_read_byte(uint32_t addr)   
{ 
     uint8_t tempdat;

    SPI_FLASH_CS_LOW;                               /* 使能器件 */
    spi_dev_transmit(W25X_ReadData);                /* 发送读取命令 */
    spi_dev_transmit((uint8_t)((addr) >> 16));      /* 发送24bit地址 */
    spi_dev_transmit((uint8_t)((addr) >> 8));   
    spi_dev_transmit((uint8_t)addr);   
    tempdat = spi_dev_receive();   /* 循环读数 */
    SPI_FLASH_CS_HIGH;             /* 取消片选 */

    return tempdat;
}

//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//addr:开始写入的地址(24bit)
//p_buf:数据存储区
//bytes:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void spi_flash_write_page(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{
     uint16_t i;

    spi_flash_write_enable();                /* SET WEL */

    SPI_FLASH_CS_LOW;                        /* 使能器件 */
    spi_dev_transmit(W25X_PageProgram);      /* 发送写页命令 */
    spi_dev_transmit((uint8_t)((addr)>>16)); /* 发送24bit地址 */
    spi_dev_transmit((uint8_t)((addr)>>8));   
    spi_dev_transmit((uint8_t)addr);   
    for (i = 0; i < bytes; i++) {
        spi_dev_transmit(p_buf[i]);          /* 循环写数 */
    }
    SPI_FLASH_CS_HIGH;                       /* 取消片选 */
    spi_flash_wait_busy();                   /* 等待写入结束 */
}

//无检验写SPI FLASH 
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能 
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//addr:开始写入的地址(24bit)
//p_buf:数据存储区
//bytes:要写入的字节数(最大65535)
void spi_flash_write_no_check(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{
    uint16_t pageremain;

    pageremain = 256 - addr % 256; /* 单页剩余的字节数 */
    if (bytes <= pageremain) {
        pageremain = bytes;        /* 不大于256个字节 */
     }
    while(1) {
        spi_flash_write_page(addr, p_buf, pageremain);
        if (bytes == pageremain) {
            break;
        } else {
            p_buf += pageremain;
            addr  += pageremain;
            bytes -= pageremain;     /* 减去已经写入了的字节数 */
            if(bytes > 256) {
                pageremain = 256;    /* 一次可以写入256个字节 */
            } else {
                pageremain = bytes;  /* 不够256个字节了 */
            }
        }
    }
}

//擦除整个芯片
//整片擦除时间:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//等待时间超长...
void spi_flash_erase_chip(void)
{
    spi_flash_write_enable();
    spi_flash_wait_busy();
    SPI_FLASH_CS_LOW;
    spi_dev_transmit(W25X_ChipErase);        /* 发送片擦除命令 */
    SPI_FLASH_CS_HIGH;
    spi_flash_wait_busy();                   /* 等待芯片擦除结束 */
}

//擦除一个扇区
//dst_addr:扇区地址
//擦除一个山区的最少时间:150ms
void spi_flash_erase_sector(uint32_t dst_addr)
{
    spi_flash_write_enable();
    spi_flash_wait_busy();   
    SPI_FLASH_CS_LOW;                               /* 使能器件 */
    spi_dev_transmit(W25X_SectorErase);             /* 发送扇区擦除指令 */
    spi_dev_transmit((uint8_t)((dst_addr) >> 16));  /* 发送24bit地址 */
    spi_dev_transmit((uint8_t)((dst_addr) >> 8));   
    spi_dev_transmit((uint8_t)dst_addr);  
    SPI_FLASH_CS_HIGH;                              /* 取消片选 */
    spi_flash_wait_busy();                          /* 等待擦除完成 */
}

/* end of file */
