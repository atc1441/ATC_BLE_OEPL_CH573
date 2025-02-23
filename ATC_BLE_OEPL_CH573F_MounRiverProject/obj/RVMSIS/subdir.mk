################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RVMSIS/core_riscv.c 

C_DEPS += \
./RVMSIS/core_riscv.d 

OBJS += \
./RVMSIS/core_riscv.o 



# Each subdirectory must supply rules for building sources it contributes
RVMSIS/%.o: ../RVMSIS/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -DDEBUG=1 -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver/inc" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/RVMSIS" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/src" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
