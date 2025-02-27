/*!
    \file    main.c
    \brief   slave receiver

    \version 2023-06-16, V1.3.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software without
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

#include <stdio.h>
#include "gd32e507z_eval.h"
#include "gd32e50x.h"

#define I2C2_OWN_ADDRESS7      0x82

uint8_t i2c_receiver[16];

void rcu_config(void);
void i2c_gpio_config(void);
void i2c_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    int i;

    /* configure USART */
    gd_eval_com_init(EVAL_COM0);
    printf("\r\n I2C Start\r\n");
    /* enable the peripheral clock */
    rcu_config();
    /* configure the I2C GPIO ports */
    i2c_gpio_config();
    /* configure I2C */
    i2c_config();

    i = 0;
    /* wait until ADDSEND bit is set */
    while(!i2c2_flag_get(I2C2, I2C2_FLAG_ADDSEND));
    /* clear ADDSEND bit */
    i2c2_flag_clear(I2C2, I2C2_FLAG_ADDSEND);
    for(i = 0; i < 16; i++) {
        /* wait until the RBNE bit is set */
        while(!i2c2_flag_get(I2C2, I2C2_FLAG_RBNE));
        /* read a data byte from I2C_RDATA */
        i2c_receiver[i] = i2c_data_receive(I2C2);
    }
    /* wait until the STPDET bit is set */
    while(!i2c2_flag_get(I2C2, I2C2_FLAG_STPDET));
    /* clear the STPDET bit */
    i2c2_flag_clear(I2C2, I2C2_FLAG_STPDET);
    for(i = 0; i < 16; i++) {
        printf("0x%02X ", i2c_receiver[i]);
    }
    while(1) {
    }
}

/*!
    \brief      enable the peripheral clock
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable AFIO clock */
    rcu_periph_clock_enable(RCU_AF);
    /* enable I2C2 clock */
    rcu_periph_clock_enable(RCU_I2C2);
}

/*!
    \brief      configure the I2C2 GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_gpio_config(void)
{
    /* configure AFIO port alternate function */
    gpio_afio_port_config(AFIO_PC9_I2C2_CFG, ENABLE);
    gpio_afio_port_config(AFIO_PA8_I2C2_CFG, ENABLE);

    /* connect PA8 to I2C2_SCL */
    /* connect PC9 to I2C2_SDA */
    gpio_init(GPIOC, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    gpio_init(GPIOA, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
}

/*!
    \brief      configure the I2C2 interface
    \param[in]  none
    \param[out] none
    \retval     none
*/
void i2c_config(void)
{
    /* configure I2C timing */
    i2c_timing_config(I2C2, 0, 0x8, 0);
    /* configure I2C address */
    i2c_address_config(I2C2, I2C2_OWN_ADDRESS7, I2C2_ADDFORMAT_7BITS);
    /* enable I2C2 */
    i2c_enable(I2C2);
}

/* retarget the C library printf function to the usart */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(EVAL_COM0, (uint8_t)ch);
    while(RESET == usart_flag_get(EVAL_COM0, USART_FLAG_TBE));
    return ch;
}
