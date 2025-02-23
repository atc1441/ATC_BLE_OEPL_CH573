################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################
-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CH573F.hex \

SECONDARY_LIST += \
CH573F.lst \

SECONDARY_SIZE += \
CH573F.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: CH573F.elf secondary-outputs

# Tool invocations
CH573F.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -T "c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"CH573F.map" --specs=nano.specs --specs=nosys.specs -o "CH573F.elf" $(OBJS) $(USER_OBJS) $(LIBS)
CH573F.hex: CH573F.elf
	@	riscv-none-embed-objcopy -O ihex "CH573F.elf" "CH573F.hex"
CH573F.lst: CH573F.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "CH573F.elf" > "CH573F.lst"
CH573F.siz: CH573F.elf
	riscv-none-embed-size --format=berkeley "CH573F.elf"

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) CH573F.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets