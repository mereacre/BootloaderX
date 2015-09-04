/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief  Xmega Bootloader UART header file.
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

/*! \brief Generate UART initialisation section.
 *
 *  \retval None
 */
void initbootuart( void );
/*! \brief UART Transmitting section.
 *
 *  \retval None
 */
void sendchar( unsigned char );
/*! \brief Generate UART initialisation section.
 *
 *  \retval 8-bit(unsigned char) Received Character
 */
unsigned char recchar( void );

