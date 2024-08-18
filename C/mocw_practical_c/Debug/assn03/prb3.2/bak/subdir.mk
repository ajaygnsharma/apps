################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../assn03/prb3.2/bak/simpleinit.c 

OBJS += \
./assn03/prb3.2/bak/simpleinit.o 

C_DEPS += \
./assn03/prb3.2/bak/simpleinit.d 


# Each subdirectory must supply rules for building sources it contributes
assn03/prb3.2/bak/%.o: ../assn03/prb3.2/bak/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


