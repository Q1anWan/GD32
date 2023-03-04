#include "bsp_spi.h"

void spi0_init(void)
{
    spi_parameter_struct  spi_init_struct;

    rcu_periph_clock_enable(RCU_SPI0);
    rcu_periph_clock_enable(RCU_GPIOB);

    /* ≈‰÷√CS“˝Ω≈ PB6 */
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6);
    gpio_bit_set(GPIOB, GPIO_PIN_6);

    /* SPI0“˝Ω≈≈‰÷√£¨CLK:PB3  MOSI:PB5  MISO:PB4 */
    gpio_af_set(GPIOB, GPIO_AF_0, GPIO_PIN_3 | GPIO_PIN_4 |GPIO_PIN_5);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4 |GPIO_PIN_5);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3 | GPIO_PIN_4 |GPIO_PIN_5);

    /* SPI0 parameter config */
    spi_i2s_deinit(SPI0);
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);	
	spi_enable(SPI0);
}

uint8_t spi0_rw_byte(uint8_t byte)
{
    /* loop while data register in not emplty */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE));

    /* send byte through the SPI1 peripheral */
    spi_i2s_data_transmit(SPI0, byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE));

    /* return the byte read from the SPI bus */
    return((uint8_t)spi_i2s_data_receive(SPI0));
}

void spi0_cs_set(int value)
{
    if (value == 1) {
        gpio_bit_set(GPIOB, GPIO_PIN_6);
    } else {
        gpio_bit_reset(GPIOB, GPIO_PIN_6);
    }
}

/* end of file */
