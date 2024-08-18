################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include quiz/03/subdir.mk
-include quiz/02/subdir.mk
-include quiz/subdir.mk
-include assn05/prb5.1/subdir.mk
-include assn04/prb4.2/subdir.mk
-include assn04/prb4.1/subdir.mk
-include assn03/prb3.5/Debug/subdir.mk
-include assn03/prb3.5/subdir.mk
-include assn03/prb3.4/subdir.mk
-include assn03/prb3.3/subdir.mk
-include assn03/prb3.2/bak/subdir.mk
-include assn03/prb3.2/subdir.mk
-include assn03/prb3.1/subdir.mk
-include assn02/src/subdir.mk
-include assn02/lib/subdir.mk
-include assn01/src/subdir.mk
-include assn01/lib/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: mocw_pc

# Tool invocations
mocw_pc: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	gcc  -o "mocw_pc" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) mocw_pc
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
