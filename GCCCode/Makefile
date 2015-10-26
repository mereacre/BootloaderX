################################################################################
# Automatically-generated file. Do not edit!
################################################################################
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
main.c \
nvm.c \
serial.c


PREPROCESSING_SRCS +=  \
ccp.s \
nvm_asm.s


ASM_SRCS += 


OBJS +=  \
ccp.o \
main.o \
nvm.o \
nvm_asm.o \
serial.o

OBJS_AS_ARGS +=  \
ccp.o \
main.o \
nvm.o \
nvm_asm.o \
serial.o

C_DEPS +=  \
ccp.d \
main.d \
nvm.d \
nvm_asm.d \
serial.d

C_DEPS_AS_ARGS +=  \
ccp.d \
main.d \
nvm.d \
nvm_asm.d \
serial.d

OUTPUT_FILE_PATH +=BootloaderX.elf

OUTPUT_FILE_PATH_AS_ARGS +=BootloaderX.elf

ADDITIONAL_DEPENDENCIES:=

# OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler







./%.o: ./%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.8.1
	avr-gcc -x c -funsigned-char -funsigned-bitfields -DNDEBUG  -Os -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -Wall -mmcu=atxmega384c3 -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler
./ccp.o: ./ccp.s
	@echo Building file: $<
	@echo Invoking: AVR32/GNU Assembler : 4.8.1
	avr-gcc -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega384c3 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -mrelax   -o "$@" "$<" 
	@echo Finished building: $<
	

./nvm_asm.o: ./nvm_asm.s
	@echo Building file: $<
	@echo Invoking: AVR32/GNU Assembler : 4.8.1
	avr-gcc -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega384c3 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -mrelax   -o "$@" "$<" 
	@echo Finished building: $<
	


./%.o: ./%.s
	@echo Building file: $<
	@echo Invoking: AVR32/GNU Assembler : 4.8.1
	avr-gcc -Wa,-gdwarf2 -x assembler-with-cpp -c -mmcu=atxmega384c3 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -mrelax   -o "$@" "$<" 
	@echo Finished building: $<
	




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.8.1
	avr-gcc -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="BootloaderX.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=atxmega384c3 -Wl,--section-start=.text=0x60000  
	@echo Finished building target: $@
	avr-objcopy -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "BootloaderX.elf" "BootloaderX.hex"
	avr-objcopy -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "BootloaderX.elf" "BootloaderX.eep" || exit 0
	avr-objdump -h -S "BootloaderX.elf" > "BootloaderX.lss"
	avr-objcopy -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "BootloaderX.elf" "BootloaderX.srec"
	avr-size "BootloaderX.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "BootloaderX.elf" "BootloaderX.a" "BootloaderX.hex" "BootloaderX.lss" "BootloaderX.eep" "BootloaderX.map" "BootloaderX.srec" "BootloaderX.usersignatures"
	