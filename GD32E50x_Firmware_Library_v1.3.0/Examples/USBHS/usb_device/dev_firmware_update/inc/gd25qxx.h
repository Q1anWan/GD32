/*!
    \file    gd25qxx.h
    \brief   the header file of SPI flash gd25qxx driver

    \version 2023-06-16, V1.3.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    All rights reserved.

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

#ifndef GD25QXX_H
#define GD25QXX_H

#include "gd32e50x.h"

/* initialize SQPI parameter */
void sqpi_flash_init(void);
/* enable flash quad operation */
void sqpi_flash_quad_enable(void);
/* read flash identification */
uint32_t sqpi_flash_read_id(void);
/* erase the specified flash block */
void sqpi_flash_block_erase(uint32_t block_addr);
/* erase the specified flash sector */
void sqpi_flash_sector_erase(uint32_t sector_addr);
/* erase the entire flash */
void sqpi_flash_chip_erase(void);
/* read a block of data from the flash using SQPI */
void sqpi_flash_buffer_read(uint32_t read_addr,uint8_t * read_buff,uint32_t size);
/* write block of data to the flash using SQPI */
void sqpi_flash_buffer_write(uint32_t write_addr, uint8_t* pbuffer, uint16_t num_byte_to_write);


#endif /* GD25QXX_H */
