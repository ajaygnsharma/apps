################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../quiz/mit_c_quiz_01.c 

OBJS += \
./quiz/mit_c_quiz_01.o 

C_DEPS += \
./quiz/mit_c_quiz_01.d 


# Each subdirectory must supply rules for building sources it contributes
quiz/%.o: ../quiz/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


