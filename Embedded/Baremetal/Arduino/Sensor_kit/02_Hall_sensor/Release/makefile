################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -f

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include core/core/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(SX_DEPS)),)
-include $(SX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
AR += \
/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release/arduino.ar \

ELF += \
arduino_03_hall_sensor.elf \

HEX += \
arduino_03_hall_sensor.hex \



#bootloaderTest
BurnBootLoader: 
	@echo trying to burn bootloader ${bootloader.tool}
	${tools.${bootloader.tool}.erase.pattern}
	${tools.${bootloader.tool}.bootloader.pattern}

uploadWithBuild: all
	@echo trying to build and upload with upload tool ${upload.tool}
	${tools.${upload.tool}.upload.pattern}

uploadWithoutBuild: 
	@echo trying to upload without build with upload tool ${upload.tool}
	${tools.${upload.tool}.upload.pattern}
    
uploadWithProgrammerWithBuild: all
	@echo trying to build and upload with programmer ${program.tool}
	${tools.${program.tool}.program.pattern}

uploadWithProgrammerWithoutBuild: 
	@echo trying to upload with programmer ${program.tool} without build
	${tools.${program.tool}.program.pattern}

# All Target
all: arduino_03_hall_sensor

# Tool invocations
arduino_03_hall_sensor: $(HEX)
	@echo 'Building target: $@'
	@echo 'Printing size:'
	"/home/asharma/eclipse/embedcpp-2024-03/eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-size" -A "/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release/arduino_03_hall_sensor.elf"
	@echo 'Finished building target: $@'
	@echo ' '

/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release/arduino.ar: $(AR_OBJ)
	@echo 'Starting archiver'
	"/home/asharma/eclipse/embedcpp-2024-03/eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc-ar" rcs  "/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release/arduino.ar" $(AR_OBJ)
	@echo 'Finished building: $@'
	@echo ' '

arduino_03_hall_sensor.elf: $(LINK_OBJ) $(AR)
	@echo 'Starting combiner'
	"/home/asharma/eclipse/embedcpp-2024-03/eclipse/arduinoPlugin/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin/avr-gcc" -w -Os -g -flto -fuse-linker-plugin -Wl,--gc-sections,--relax -mmcu=atmega2560   -o "/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release/arduino_03_hall_sensor.elf"  $(LINK_OBJ)  $(AR) "-L/home/asharma/eclipse-workspace-2024/arduino_03_hall_sensor/Release" -lm
	@echo 'Finished building: $@'
	@echo ' '

arduino_03_hall_sensor.hex: $(ELF)
	@echo 'Do all objcopy commands'
	${recipe.objcopy.eep.pattern}
	${recipe.objcopy.hex.pattern}
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(CCM_DEPS)$(C_UPPER_DEPS)$(PDE_DEPS)$(CXXM_DEPS)$(C_DEPS)$(AR)$(CC_DEPS)$(AR_OBJ)$(SX_DEPS)$(C++_DEPS)$(LINK_OBJ)$(CXX_DEPS)$(ASM_DEPS)$(HEX)$(INO_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(C++M_DEPS)$(ELF)$(CPP_DEPS)
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
