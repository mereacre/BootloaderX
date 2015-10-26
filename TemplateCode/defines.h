/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief  Xmega Bootloader Definitions
*
* \par Application note:
*      AVR1605: Xmega Bootloader
*
* \author
*      Atmel Corporation: http://www.atmel.com \n
*      Support email: avr@atmel.com
*
* $Revision: 4735 $
* $Date: 2014-03-12 \n

*Copyright (c) 2014, Atmel Corporation All rights reserved.
*
* \page License
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
* 3. The name of Atmel may not be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
* 4. This software may only be redistributed and used in connection with an
*    Atmel microcontroller product.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
* EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
* ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
******************************************************************************/

#ifndef	INCLUDE_DEFINES_H	
#define	INCLUDE_DEFINES_H
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

/* baud rate register value calculation */
#define	BRREG_VALUE	12 //12 = 9600bps for 2MHz clock

/* definitions for UART control */
#define UART_PORT               PORTD
#define UART_TX_PIN             PIN3_bm
#define	BAUD_RATE_LOW_REG		USARTD0.BAUDCTRLA
#define	UART_CONTROL_REG		USARTD0.CTRLB
#define	ENABLE_TRANSMITTER_BIT	USART_TXEN_bp
#define	ENABLE_RECEIVER_BIT		USART_RXEN_bp
#define	UART_STATUS_REG			USARTD0.STATUS
#define	TRANSMIT_COMPLETE_BIT	USART_TXCIF_bp
#define	DATA_REG_EMPTY_BIT		USART_DREIF_bp
#define	RECEIVE_COMPLETE_BIT	USART_RXCIF_bp
#define	UART_DATA_REG			USARTD0.DATA

#endif	/* INCLUDE_DEFINES_H */
