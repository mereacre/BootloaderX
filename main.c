/* This file has been prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
*
* \brief  Xmega Bootloader
*
*      This Program allows an AVR Xmega to Read/write its
*      own Flash/EEprom. To enter Programming mode an input pin
*      is checked. If this pin is pulled low, programming mode 
*      is entered. If not, normal execution is done from $0000 
*      "reset" vector in Application area.
*
* \par Application note:
*      AVR1605: Xmega Bootloader
*
*
* \par Documentation
*      For comprehensive code documentation, supported compilers, compiler
*      settings and supported devices see readme.html
*
* \author
*      Atmel Corporation: http://www.atmel.com \n
*      Support email: avr@atmel.com
*
* $Revision: 2748 $
* $Date: 2014-03-12 
*
* Copyright (c) 2014, Atmel Corporation All rights reserved.
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

#include "defines.h"
#include "serial.h"
#include <avr/io.h>
#include "nvm.h"
#include "clksys_driver.h"

#define ADDR_T unsigned long

#ifndef REMOVE_BLOCK_SUPPORT
unsigned char BlockLoad(unsigned int size, unsigned char mem, ADDR_T address);
void BlockRead(unsigned int size, unsigned char mem, ADDR_T address);
#endif /* REMOVE_BLOCK_SUPPORT */

#ifdef __ICCAVR__
#  define C_TASK __C_task
#else /* ! __ICCAVR__ */
#  define C_TASK /**/
#endif /* __ICCAVR__ */
uint8_t WriteBuffer[FLASH_PAGE_SIZE];
uint8_t ReadBuffer[FLASH_PAGE_SIZE];

/* Set interrupt vector location to boot section of flash */
void PMIC_SetVectorLocationToBoot( void )
{
   uint8_t temp = PMIC.CTRL | PMIC_IVSEL_bm;
   CCP = CCP_IOREG_gc;
   PMIC.CTRL = temp;
}

/*Set interrupt vector location to application section of flash */
void PMIC_SetVectorLocationToApplication( void )
{
   uint8_t temp = PMIC.CTRL & ~PMIC_IVSEL_bm;
   CCP = CCP_IOREG_gc;
   PMIC.CTRL = temp;
}

/* NVM-workaround code (for ATxmega256A3 rev B) */
#ifdef WORKAROUND
/* Temporary register storage */
uint8_t sleepCtr;
uint8_t statusStore;
uint8_t pmicStore;
uint8_t globalInt;
uint8_t spmintStore;

/* SPM wakeup interrupt */
ISR(NVM_SPM_vect)
{
   /* Disable the SPM interrupt */
   NVM.INTCTRL = (NVM.INTCTRL & ~NVM_SPMLVL_gm);
   /* Restore sleep settings */
   SLEEP.CTRL = sleepCtr;
   /* Restore PMIC status and control registers */
   PMIC.STATUS = statusStore;
   PMIC.CTRL = pmicStore;
   /* Restore SPM interruptsettings */
   NVM.INTCTRL = spmintStore;
   /* Restore global interrupt settings */
   SREG = globalInt;
   return;
}

/* EEPROM wakeup interrupt */
ISR(NVM_EE_vect)
{
   /* Disable the EEPROM interrupt */
   NVM.INTCTRL = (NVM.INTCTRL & ~NVM_EELVL_gm);
   /* Restore sleep settings */
   SLEEP.CTRL = sleepCtr;
   /* Restore PMIC status and control registers */
   PMIC.STATUS = statusStore;
   PMIC.CTRL = pmicStore;
   /* Restore SPM interruptsettings */
   NVM.INTCTRL = spmintStore;
   /* Restore global interrupt settings */
   SREG = globalInt;
   return;
}

/* Save register settings before entering sleep mode */
void Prepare_to_Sleep( void )
{
   sleepCtr = SLEEP.CTRL;
   /* Set sleep mode to IDLE */
   SLEEP.CTRL = SLEEP_SMODE_IDLE_gc;
   /* Save the PMIC Status and control registers */
   statusStore = PMIC.STATUS;								
   pmicStore = PMIC.CTRL;		
   /* Enable only the highest level of interrupts */									
   PMIC.CTRL = (PMIC.CTRL & ~PMIC_HILVLEN_bm) | PMIC_HILVLEN_bm;
   /* Save SREG for later use */
   globalInt = SREG;
   /* Enable global interrupts */
   sei();
   /* Save SPM interrupt settings for later */ 
   spmintStore = NVM.INTCTRL;
}
#endif


/* New function declarations used for the NVM-workaround */
void EraseApplicationPage(uint32_t address)
{
   /*Set the correct settings and store critical registers before NVM-workaround*/
#ifdef WORKAROUND
   Prepare_to_Sleep();
#endif
   /*Assembly "function" to preform page erase*/
    nvm_flash_erase_app_page(address);
}

void EraseWriteApplicationPage(uint32_t address)
{
   /*Set the correct settings and store critical registers before NVM-workaround*/
#ifdef WORKAROUND
   Prepare_to_Sleep();
#endif
   /*Assembly "function" to preform page erase-write*/
   nvm_flash_atomic_write_app_page(address); 
}
void WriteApplicationPage(uint32_t address)
{
   /*Set the correct settings and store critical registers before NVM-workaround*/
#ifdef WORKAROUND
   Prepare_to_Sleep();
#endif
   /*Assembly "function" to preform page write*/
   nvm_flash_split_write_app_page(address);
}

void LoadFlashWord(uint32_t address, uint16_t word )
{
   /*Set the correct settings and store critical registers before NVM-workaround*/
#ifdef WORKAROUND
   Prepare_to_Sleep();
#endif
   /*Assembly "function" to load word into flash buffer*/
  nvm_flash_load_word_to_buffer(address, word);
}

void EraseFlashBuffer(void)
{
   /*Set the correct settings and store critical registers before NVM-workaround*/
#ifdef WORKAROUND
   Prepare_to_Sleep();
#endif
   /*Assembly "function" to erase flash buffer*/
    nvm_flash_flush_buffer();
}

int main(void)
{
   ADDR_T address = 0;
   unsigned int temp_int=0;
   unsigned char val;
   
   /* Initialization */    
   void (*funcptr)( void ) = 0x0000; // Set up function pointer to RESET vector.
   
   PMIC_SetVectorLocationToBoot();
   
   // Configure 32KHz OSC
   CLKSYS_Enable( OSC_RC32KEN_bm );
   do {} while ( CLKSYS_IsReady( OSC_RC32KEN_bm ) == 0 );    
   CLKSYS_Enable( OSC_RC32MEN_bm );
   do {} while ( CLKSYS_IsReady( OSC_RC32MRDY_bm ) == 0 );
   CLKSYS_Main_ClockSource_Select( CLK_SCLKSEL_RC32M_gc );
   CLKSYS_Disable( OSC_RC2MEN_bm);
   CLKSYS_AutoCalibration_Enable( OSC_RC32MCREF0_bm, true );
   
   eeprom_disable_mapping();
   
   PROGPORT |= (1<<PROG_NO); // Enable pull-up on PROG_NO line on PROGPORT.
   
   /* Branch to bootloader or application code? */
   if( /*!(PROGPIN & (1<<PROG_NO))*/1 ) // If PROGPIN is pulled low, enter programming mode.
   {
      initbootuart(); // Initialize UART.

      /* Main loop */
      for(;;)
      {
	 
	 val = recchar(); // Wait for command character.
	 
	 // Check autoincrement status.
	 if(val=='a')
	 {
	    sendchar('Y'); // Yes, we do autoincrement.
	 }
	 
	 // Set address (2 bytes).
	 else if(val == 'A')
	 { // NOTE: Flash addresses are given in words, not bytes.                                            
	    address = recchar();
	    address <<=  8;
	    address |= recchar(); // Read address high and low byte.
	    sendchar('\r'); // Send OK back.
	 }

	 // Set extended address (3 bytes).
	 else if(val == 'H')
	 { // NOTE: Flash addresses are given in words, not bytes.                                            
	    address = (uint32_t)recchar() << 16;
	    address |= (uint16_t)recchar() << 8;
	    address |= recchar();
	    sendchar('\r'); // Send OK back.
	 }

	 // Chip erase.
	 else if(val=='e')
	 {
	    for(address = 0; address < APP_END; address += PAGESIZE)
	    { // NOTE: Here we use address as a byte-address, not word-address, for convenience.
	       nvm_wait_until_ready();
#ifdef __ICCAVR__
#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
#endif
	       EraseApplicationPage( address );
#ifdef __ICCAVR__
#pragma diag_default=Pe1053 // Back to default.
#endif
	    }
	    nvm_eeprom_erase_all();
	    sendchar('\r'); // Send OK back.
	 }
	 
#ifndef REMOVE_BLOCK_SUPPORT

	 // Check block load support.
	 else if(val=='b')
	 {
	    sendchar('Y'); // Report block load supported.
	    sendchar((BLOCKSIZE>>8) & 0xFF); // MSB first.
	    sendchar(BLOCKSIZE&0xFF); // Report BLOCKSIZE (bytes).
	 }
	 
	 // Start block load.
	 else if(val=='B')
	 {
	    temp_int = ((uint16_t)recchar()<<8) | recchar(); // Get block size.
	    val = recchar(); // Get memtype.
	    sendchar( BlockLoad(temp_int, val, address) ); // Block load.				
	 }	    
	 // Start block read.
	 else if(val=='g')
	 {
	    temp_int = ((uint16_t)recchar()<<8) | recchar(); // Get block size.
	    val = recchar(); // Get memtype
	    BlockRead(temp_int, val, address); // Block read
	 }		
#endif /* REMOVE_BLOCK_SUPPORT */
	 
#ifndef REMOVE_FLASH_BYTE_SUPPORT            
	 // Read program memory.
	 else if(val=='R')
	 {        
	    // Send high byte, then low byte of flash word.
	    nvm_wait_until_ready();
	    sendchar(nvm_flash_read_byte( (address << 1)+1) );
	    sendchar( nvm_flash_read_byte( (address << 1)+0) );
	    address++; // Auto-advance to next Flash word.
	 }
	 
	 // Write program memory, low byte.        
	 else if(val=='c')
	 { // NOTE: Always use this command before sending high byte.
	    temp_int=recchar(); // Get low byte for later LoadFlashWord
	    sendchar('\r'); // Send OK back.
	 }
	 
	 // Write program memory, high byte.
	 else if(val=='C')
	 {
	    temp_int |= (uint16_t)recchar()<<8; // Get and insert high byte.
	    nvm_wait_until_ready();
	    LoadFlashWord( (address << 1), temp_int );
	    address++; // Auto-advance to next Flash word.
	    sendchar('\r'); // Send OK back.
	 }
	 
	 // Write page.       
	 else if(val== 'm')
	 {
	    if( address >= (APP_END>>1) ) // Protect bootloader area.		    
	    {
	       sendchar('?');
	    } 
	    else
	    {
	       nvm_wait_until_ready();
	       // Convert word-address to byte-address and write.
	       EraseWriteApplicationPage( address << 1);
	       sendchar('\r'); // Send OK back.
	    }
#endif // REMOVE_FLASH_BYTE_SUPPORT
	    
#ifndef REMOVE_EEPROM_BYTE_SUPPORT
	 }
	 // Write EEPROM memory.
	 else if (val == 'D')
	 {
	    nvm_eeprom_write_byte(address,recchar());
	    // Select next EEPROM byte
	    address++;
	 }
	 
	 // Read EEPROM memory.
	 else if (val == 'd')
	 {
	    sendchar(nvm_eeprom_read_byte(address));
	    // Select next EEPROM byte
	    address++;
	 }
	 
#endif 

	 
#ifndef REMOVE_AVRPROG_SUPPORT        
	 // Enter and leave programming mode.
	 else if((val=='P')||(val=='L'))
	 {
	    sendchar('\r'); // Nothing special to do, just answer OK.
	 }
	 // Exit bootloader.
	 else if(val=='E')
	 {
	    nvm_wait_until_ready();
	    sendchar('\r');
	    PMIC_SetVectorLocationToApplication();
	    EIND = 0x00;
	    funcptr(); // Jump to Reset vector 0x0000 in Application Section.
	 }
	 // Get programmer type.        
	 else if (val=='p')
	 {
	    sendchar('S'); // Answer 'SERIAL'.
	 }
	 // Return supported device codes.
	 else if(val=='t')
	 {
#if PARTCODE+0 > 0
	    sendchar( PARTCODE ); // Supports only this device, of course.
#endif /* PARTCODE */
	    sendchar( 0 ); // Send list terminator.
	 }
	 // Set LED, clear LED and set device type.
	 else if((val=='x')||(val=='y')||(val=='T'))
	 {
	    recchar(); // Ignore the command and it's parameter.
	    sendchar('\r'); // Send OK back.
	 }
#endif /* REMOVE_AVRPROG_SUPPORT */
	 // Return programmer identifier.
	 else if(val=='S')
	 {
	    sendchar('A'); // Return 'AVRBOOT'.
	    sendchar('V'); // Software identifier (aka programmer signature) is always 7 characters.
	    sendchar('R');
	    sendchar('B');
	    sendchar('O');
	    sendchar('O');
	    sendchar('T');
	 }
	 // Return software version.
	 else if(val=='V')
	 {
	    sendchar('1');
	    sendchar('7');
	 }        
	 // Return signature bytes.
	 else if(val=='s')
	 {							
	    sendchar( SIGNATURE_BYTE_3 );
	    sendchar( SIGNATURE_BYTE_2 );
	    sendchar( SIGNATURE_BYTE_1 );
	 }       
	 // The last command to accept is ESC (synchronization).
	 else if(val!=0x1b) // If not ESC, then it is unrecognized...
	 {
	    sendchar('?');
	 }
      } // end: for(;;)
   }
   else
   {
      nvm_wait_until_ready();
      
      PMIC_SetVectorLocationToApplication();
      EIND = 0x00;
      funcptr(); // Jump to Reset vector 0x0000 in Application Section.
   }
} // end: main


#ifndef REMOVE_BLOCK_SUPPORT

unsigned char BlockLoad(unsigned int size, unsigned char mem, ADDR_T address)
{   
   unsigned int data;    
   ADDR_T tempaddress;
   
   // EEPROM memory type.
   if(mem=='E')
   {
      unsigned char value;
      unsigned char buffer[BLOCKSIZE];
      
      
	  nvm_eeprom_flush_buffer();
      // disable mapping of EEPROM into data space (enable IO mapped access)
      
	  eeprom_disable_mapping();
      
      // Fill buffer first, as EEPROM is too slow to copy with UART speed 
      for(tempaddress=0;tempaddress<size;tempaddress++){
	 buffer[tempaddress] = recchar();
      }
      
      // Then program the EEPROM 
      for( tempaddress=0; tempaddress < size; tempaddress++)
      {
	 	 value = buffer[tempaddress];
	 
	 
	 nvm_eeprom_write_byte(address,value);
	 
	 (address)++; // Select next EEPROM byte
      }
      
      return '\r'; // Report programming OK
   } 
   
   // Flash memory type
   else if(mem=='F')
   { // NOTE: For flash programming, 'address' is given in words.
      address <<= 1; // Convert address to bytes temporarily.
      EraseFlashBuffer(); //Erase the flash buffer to avoid data corruption
      for (int i = 0; i < size; i +=2) {
	 data = recchar();
	 data |= (uint16_t)recchar() << 8;
	 
	 nvm_wait_until_ready();
#ifdef __ICCAVR__
#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
#endif
	 LoadFlashWord(i, data);
#ifdef __ICCAVR__
#pragma diag_default=Pe1053 // Back to default.
#endif
      }
      
#ifdef __ICCAVR__
#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
#endif
      EraseWriteApplicationPage(address);
 
#ifdef __ICCAVR__
#pragma diag_default=Pe1053 // Back to default.
#endif        
      nvm_wait_until_ready();
      
      return '\r'; // Report programming OK
   }
   
   // Invalid memory type?
   else
   {
      return '?';
   }
}


void BlockRead(unsigned int size, unsigned char mem, ADDR_T address)
{
   // EEPROM memory type.
   
   if (mem=='E') // Read EEPROM
   {
      eeprom_disable_mapping();
      
	  nvm_eeprom_flush_buffer();
      
      do
      {
	 sendchar(nvm_eeprom_read_byte(address));
	 // Select next EEPROM byte
	 (address)++;            
	 size--; // Decrease number of bytes to read
      } while (size); // Repeat until all block has been read
   }
   
   // Flash memory type.
   else if(mem=='F')
   {
      (address) <<= 1; // Convert address to bytes temporarily.
      
      do
      {
#ifdef __ICCAVR__
#pragma diag_suppress=Pe1053 // Suppress warning for conversion from long-type address to flash ptr.
#endif
	 sendchar( nvm_flash_read_byte( address) );
	 sendchar( nvm_flash_read_byte( (address)+1) );
#ifdef __ICCAVR__
#pragma diag_default=Pe1053     // Back to default.
#endif
	 (address) += 2;    // Select next word in memory.
	 size -= 2;          // Subtract two bytes from number of bytes to read
      } while (size);         // Repeat until all block has been read
      
      (address) >>= 1;       // Convert address back to Flash words again.
   }
}

#endif /* REMOVE_BLOCK_SUPPORT */
/* end of file */
