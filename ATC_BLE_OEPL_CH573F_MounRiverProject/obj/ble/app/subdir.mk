################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ble/app/peripheral.c 

C_DEPS += \
./ble/app/peripheral.d 

OBJS += \
./ble/app/peripheral.o 



# Each subdirectory must supply rules for building sources it contributes
ble/app/%.o: ../ble/app/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -DDEBUG=1 -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver/inc" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/RVMSIS" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/ble" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/ble/app" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/ble/hal" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/ble/lib" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/ble/profile" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
