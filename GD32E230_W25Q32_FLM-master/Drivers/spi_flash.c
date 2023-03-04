#include "spi_flash.h" 
#include "bsp_spi.h"
#include "string.h"

/* SPI Flash CS */
#define SPI_FLASH_CS_LOW     spi0_cs_set(0)
#define SPI_FLASH_CS_HIGH    spi0_cs_set(1)

/* SPIд�����ӿ����� */
static inline uint8_t spi_dev_transmit(uint8_t byte)
{
    return spi0_rw_byte(byte);
}

/* SPI�������ӿ����� */
static inline uint8_t spi_dev_receive(void)
{
    return spi0_rw_byte(0xFF);
}

//��ȡSPI_FLASH��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
static uint8_t spi_flash_read_state(void) 
{
    uint8_t byte = 0;

    SPI_FLASH_CS_LOW;                      /* ʹ������ */
    spi_dev_transmit(W25X_ReadStatusReg);  /* ���Ͷ�ȡ״̬�Ĵ������� */
    byte = spi_dev_receive();          	   /* ��ȡһ���ֽ� */
    SPI_FLASH_CS_HIGH;                     /* ȡ��Ƭѡ */

    return byte;
}

//SPI_FLASHдʹ��
//��WEL��λ   
static void spi_flash_write_enable(void)   
{
    SPI_FLASH_CS_LOW;                      /* ʹ������ */
    spi_dev_transmit(W25X_WriteEnable);    /* ����дʹ�� */
    SPI_FLASH_CS_HIGH;                     /* ȡ��Ƭѡ */
}

//�ȴ�����
static void spi_flash_wait_busy(void)
{
    while ((spi_flash_read_state() & 0x01) == 0x01);   /* �ȴ�BUSYλ��� */
}

//��ȡоƬID W25X16��ID:0XEF14
uint16_t spi_flash_read_id(void)
{
    uint16_t id = 0;

    SPI_FLASH_CS_LOW;
    spi_dev_transmit(0x90);  /* �Ͷ�ȡID���� */
    spi_dev_transmit(0x00); 
    spi_dev_transmit(0x00); 
    spi_dev_transmit(0x00);
    id |= spi_dev_receive() << 8;
    id |= spi_dev_receive();
    SPI_FLASH_CS_HIGH;

    return id;
}

//ָ����ַ��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//addr:��ʼ��ȡ�ĵ�ַ(24bit)
//p_buf:���ݴ洢��s
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void spi_flash_read(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{ 
     uint16_t i;

    SPI_FLASH_CS_LOW;                               /* ʹ������ */
    spi_dev_transmit(W25X_ReadData);                /* ���Ͷ�ȡ���� */
    spi_dev_transmit((uint8_t)((addr) >> 16));      /* ����24bit��ַ */
    spi_dev_transmit((uint8_t)((addr) >> 8));   
    spi_dev_transmit((uint8_t)addr);

    for (i = 0; i < bytes; i++) { 
        p_buf[i] = spi_dev_receive();  
    }

    SPI_FLASH_CS_HIGH;                              /* ȡ��Ƭѡ */
}

uint8_t spi_flash_read_byte(uint32_t addr)   
{ 
     uint8_t tempdat;

    SPI_FLASH_CS_LOW;                               /* ʹ������ */
    spi_dev_transmit(W25X_ReadData);                /* ���Ͷ�ȡ���� */
    spi_dev_transmit((uint8_t)((addr) >> 16));      /* ����24bit��ַ */
    spi_dev_transmit((uint8_t)((addr) >> 8));   
    spi_dev_transmit((uint8_t)addr);   
    tempdat = spi_dev_receive();   /* ѭ������ */
    SPI_FLASH_CS_HIGH;             /* ȡ��Ƭѡ */

    return tempdat;
}

//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//addr:��ʼд��ĵ�ַ(24bit)
//p_buf:���ݴ洢��
//bytes:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void spi_flash_write_page(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{
     uint16_t i;

    spi_flash_write_enable();                /* SET WEL */

    SPI_FLASH_CS_LOW;                        /* ʹ������ */
    spi_dev_transmit(W25X_PageProgram);      /* ����дҳ���� */
    spi_dev_transmit((uint8_t)((addr)>>16)); /* ����24bit��ַ */
    spi_dev_transmit((uint8_t)((addr)>>8));   
    spi_dev_transmit((uint8_t)addr);   
    for (i = 0; i < bytes; i++) {
        spi_dev_transmit(p_buf[i]);          /* ѭ��д�� */
    }
    SPI_FLASH_CS_HIGH;                       /* ȡ��Ƭѡ */
    spi_flash_wait_busy();                   /* �ȴ�д����� */
}

//�޼���дSPI FLASH 
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ���� 
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//addr:��ʼд��ĵ�ַ(24bit)
//p_buf:���ݴ洢��
//bytes:Ҫд����ֽ���(���65535)
void spi_flash_write_no_check(uint32_t addr, uint8_t *p_buf, uint16_t bytes)
{
    uint16_t pageremain;

    pageremain = 256 - addr % 256; /* ��ҳʣ����ֽ��� */
    if (bytes <= pageremain) {
        pageremain = bytes;        /* ������256���ֽ� */
     }
    while(1) {
        spi_flash_write_page(addr, p_buf, pageremain);
        if (bytes == pageremain) {
            break;
        } else {
            p_buf += pageremain;
            addr  += pageremain;
            bytes -= pageremain;     /* ��ȥ�Ѿ�д���˵��ֽ��� */
            if(bytes > 256) {
                pageremain = 256;    /* һ�ο���д��256���ֽ� */
            } else {
                pageremain = bytes;  /* ����256���ֽ��� */
            }
        }
    }
}

//��������оƬ
//��Ƭ����ʱ��:
//W25X16:25s 
//W25X32:40s 
//W25X64:40s 
//�ȴ�ʱ�䳬��...
void spi_flash_erase_chip(void)
{
    spi_flash_write_enable();
    spi_flash_wait_busy();
    SPI_FLASH_CS_LOW;
    spi_dev_transmit(W25X_ChipErase);        /* ����Ƭ�������� */
    SPI_FLASH_CS_HIGH;
    spi_flash_wait_busy();                   /* �ȴ�оƬ�������� */
}

//����һ������
//dst_addr:������ַ
//����һ��ɽ��������ʱ��:150ms
void spi_flash_erase_sector(uint32_t dst_addr)
{
    spi_flash_write_enable();
    spi_flash_wait_busy();   
    SPI_FLASH_CS_LOW;                               /* ʹ������ */
    spi_dev_transmit(W25X_SectorErase);             /* ������������ָ�� */
    spi_dev_transmit((uint8_t)((dst_addr) >> 16));  /* ����24bit��ַ */
    spi_dev_transmit((uint8_t)((dst_addr) >> 8));   
    spi_dev_transmit((uint8_t)dst_addr);  
    SPI_FLASH_CS_HIGH;                              /* ȡ��Ƭѡ */
    spi_flash_wait_busy();                          /* �ȴ�������� */
}

/* end of file */
