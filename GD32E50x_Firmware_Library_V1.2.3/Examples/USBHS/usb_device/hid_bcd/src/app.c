/*!
    \file    app.c
    \brief   main routine

    \version 2020-03-10, V1.0.0, firmware for GD32E50x
    \version 2020-08-26, V1.1.0, firmware for GD32E50x
    \version 2021-03-23, V1.2.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#include "drv_usb_hw.h"
#include "standard_hid_core.h"

extern hid_fop_handler fop_handler;

usb_core_driver hid_keyboard;
void usb_vbus_gpio_config(void);

/*!
    \brief      main routine will construct a USB virtual ComPort device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
#ifdef USE_ULPI_PHY
    usb_gpio_config();
#endif /* USE_ULPI_PHY */

    /* configure LEDs */
    gd_eval_led_init(LED1);
    gd_eval_led_init(LED2);
    gd_eval_led_init(LED3);
    gd_eval_led_init(LED4);

    usb_rcu_config();

    usb_timer_init();

    usb_vbus_gpio_config();

    hid_itfop_register (&hid_keyboard, &fop_handler);

    usbd_init (&hid_keyboard, &hid_desc, &usbd_hid_cb);

#ifdef USE_USB_HS
    #ifndef USE_ULPI_PHY
        pllusb_rcu_config();
    #endif /* !USE_ULPI_PHY */
#endif /* USE_USB_HS */

    usb_intr_config();

    /* check if USB device is enumerated successfully */
    while (USBD_CONFIGURED != hid_keyboard.dev.cur_status) {
    }

    while (1) {
        fop_handler.hid_itf_data_process(&hid_keyboard);
    }
}