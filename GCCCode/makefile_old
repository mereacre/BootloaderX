#
#             LUFA Library
#     Copyright (C) Dean Camera, 2014.
#
#  dean [at] fourwalledcubicle [dot] com
#           www.lufa-lib.org
#
# --------------------------------------
#         LUFA Project Makefile.
# --------------------------------------

# Run "make help" for target help.

MCU          = atmega32u4
ARCH         = AVR8
BOARD        = USBKEY
F_CPU        = 8000000UL
TARGET       = BootloaderHID
SRC          = $(TARGET).c
LD_FLAGS     = -Wl,--section-start=.text=$(BOOT_START_OFFSET)

# Flash size and bootloader section sizes of the target, in KB. These must
# match the target's total FLASH size and the bootloader size set in the
# device's fuses.
FLASH_SIZE_KB        := 16
BOOT_SECTION_SIZE_KB := 2

# Bootloader address calculation formulas
# Do not modify these macros, but rather modify the dependent values above.
CALC_ADDRESS_IN_HEX   = $(shell printf "0x%X" $$(( $(1) )) )
BOOT_START_OFFSET     = $(call CALC_ADDRESS_IN_HEX, ($(FLASH_SIZE_KB) - $(BOOT_SECTION_SIZE_KB)) * 1024 )


# Default target
all: gcc lnk objcpy objdmp nm size

gcc: $(TARGET).c
	avr-gcc -c -pipe -gdwarf-2 -g2 -mmcu=$(MCU) -fshort-enums -fno-inline-small-functions -fpack-struct -Wall -fno-strict-aliasing -funsigned-char -funsigned-bitfields -ffunction-sections -I. -DARCH=ARCH_$(ARCH) -DBOARD=$(BOARD) -DF_CPU=$(F_CPU) -mrelax -fno-jump-tables -x c -Os -std=gnu99 -Wstrict-prototypes  -MMD -MP -MF $(TARGET).d $(TARGET).c -o $(TARGET).o

lnk: $(TARGET).o
	avr-gcc $(TARGET).o -o $(TARGET).elf -lm -Wl,-Map=$(TARGET).map,--cref -Wl,--gc-sections -Wl,--relax -mmcu=$(MCU) $(LD_FLAGS)

objcpy: $(TARGET).elf 
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature $(TARGET).elf $(TARGET).hex
	avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings $(TARGET).elf $(TARGET).eep || exit 0
	avr-objcopy -O binary -R .eeprom -R .fuse -R .lock -R .signature $(TARGET).elf $(TARGET).bin

objdmp: $(TARGET).elf
	avr-objdump -h -d -S -z $(TARGET).elf > $(TARGET).lss

nm:	$(TARGET).elf
	avr-nm -n $(TARGET).elf > $(TARGET).sym

size:	$(TARGET).elf
	avr-size --mcu=$(MCU) --format=avr $(TARGET).elf

clean:
	rm -f *.d *.eep *.elf *.lss *.map *.hex *.bin *.o *.sym
