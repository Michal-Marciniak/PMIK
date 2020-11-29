################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
PMIK_zegar_mikroprocesorowy.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
PMIK_zegar_mikroprocesorowy.list \

OBJCOPY_BIN += \
PMIK_zegar_mikroprocesorowy.bin \


# All Target
all: PMIK_zegar_mikroprocesorowy.elf secondary-outputs

# Tool invocations
PMIK_zegar_mikroprocesorowy.elf: $(OBJS) $(USER_OBJS) C:\Users\micha\STM32CubeIDE\workspace_1.4.0\PMIK_zegar_mikroprocesorowy\STM32F401RETX_FLASH.ld
	arm-none-eabi-gcc -o "PMIK_zegar_mikroprocesorowy.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\micha\STM32CubeIDE\workspace_1.4.0\PMIK_zegar_mikroprocesorowy\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="PMIK_zegar_mikroprocesorowy.map" -Wl,--gc-sections -static -u _printf_float -u _scanf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

PMIK_zegar_mikroprocesorowy.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "PMIK_zegar_mikroprocesorowy.list"
	@echo 'Finished building: $@'
	@echo ' '

PMIK_zegar_mikroprocesorowy.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "PMIK_zegar_mikroprocesorowy.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
