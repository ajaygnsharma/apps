################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../assn03/prb3.4/wc.c 

OBJS += \
./assn03/prb3.4/wc.o 

C_DEPS += \
./assn03/prb3.4/wc.d 


# Each subdirectory must supply rules for building sources it contributes
assn03/prb3.4/%.o: ../assn03/prb3.4/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


