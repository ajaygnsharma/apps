################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../assn02/lib/my_bit_lib.c \
../assn02/lib/my_char_lib.c \
../assn02/lib/my_limits.c 

OBJS += \
./assn02/lib/my_bit_lib.o \
./assn02/lib/my_char_lib.o \
./assn02/lib/my_limits.o 

C_DEPS += \
./assn02/lib/my_bit_lib.d \
./assn02/lib/my_char_lib.d \
./assn02/lib/my_limits.d 


# Each subdirectory must supply rules for building sources it contributes
assn02/lib/%.o: ../assn02/lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


