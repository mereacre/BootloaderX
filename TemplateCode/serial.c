/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief  Xmega Bootloader UART source file.
*
* \par Application note:
*      AVR1605: Xmega Bootloader
*
* \par Documentation
*      For comprehensive code documentation, supported compilers, compiler
*      settings and supported devices see readme.html
*
* \author
*      Atmel Corporation: http://www.atmel.com \n
*      Support email: avr@atmel.com
*
* $Revision: 2747 $
* $Date: 2009-09-02 18:57:33 +0530 (Wed, 02 Sep 2009) $  \n
*
* Copyright (c) 2009, Atmel Corporation All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright notice,
* this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright notice,
* this list of conditions and the following disclaimer in the documentation
* and/or other materials provided with the distribution.
*
* 3. The name of ATMEL may not be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
* SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#include "defines.h"
/*! \brief Initializing UART communcation.
*
*  This function initializes the UART communication with generic parameters as mentioned below. 
*  Both Enabling both TRASMISSION and RECEPTION
*  BAUD RATE configured to BRREG_VALUE
*  As this is important function of initializing the UART, it has to be called prior to start the communication.
*
*/
void inituart(void)
{
   UART_PORT.DIRSET |= UART_TX_PIN;
   BAUD_RATE_LOW_REG = BRREG_VALUE;
   UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
      (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit 
}

/*! \brief Transmitting a character UART communcation.
*
*  This function takes the unsigned char input given to the function and transmits out in the UART communication.
*
*  This function is called whenever a single character has to be transmitted in the UART communication.
*  \param  c     Character value to be transmitted.
*
*/
void sendchar(unsigned char c)
{ 
   UART_DATA_REG = c; // prepare transmission
   while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));
   // wait until byte sendt
   UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT); // delete TXCflag
}

/*! \brief Receiving a character in UART communcation.
*
*  This function confirms the reception of data in UART, receives that character and returns the received character to the called function.
*
*  This function is called whenever a single charater has to be received from the UART communication.
*
*  \return  Character value received from UART communication.
*/

unsigned char recchar(void)
{
   unsigned char ret;
   while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)));  // wait for data
   ret = UART_DATA_REG;
   return ret;
}

