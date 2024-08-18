################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../assn03/prb3.1/prob1.c 

OBJS += \
./assn03/prb3.1/prob1.o 

C_DEPS += \
./assn03/prb3.1/prob1.d 


# Each subdirectory must supply rules for building sources it contributes
assn03/prb3.1/%.o: ../assn03/prb3.1/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


