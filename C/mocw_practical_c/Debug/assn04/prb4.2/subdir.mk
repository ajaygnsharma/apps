################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../assn04/prb4.2/main.c \
../assn04/prb4.2/strtok.c 

OBJS += \
./assn04/prb4.2/main.o \
./assn04/prb4.2/strtok.o 

C_DEPS += \
./assn04/prb4.2/main.d \
./assn04/prb4.2/strtok.d 


# Each subdirectory must supply rules for building sources it contributes
assn04/prb4.2/%.o: ../assn04/prb4.2/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


