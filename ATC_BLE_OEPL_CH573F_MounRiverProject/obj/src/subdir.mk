################################################################################
# MRS Version: 2.1.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MCU.c \
../src/RTC.c \
../src/SLEEP.c \
../src/app_bleCmdHandle.c \
../src/drawing.c \
../src/eeprom.c \
../src/epd.c \
../src/epd_ssd.c \
../src/epd_uc.c \
../src/gattprofile.c \
../src/main.c \
../src/nfc.c \
../src/peripheral.c \
../src/settings.c 

C_DEPS += \
./src/MCU.d \
./src/RTC.d \
./src/SLEEP.d \
./src/app_bleCmdHandle.d \
./src/drawing.d \
./src/eeprom.d \
./src/epd.d \
./src/epd_ssd.d \
./src/epd_uc.d \
./src/gattprofile.d \
./src/main.d \
./src/nfc.d \
./src/peripheral.d \
./src/settings.d 

OBJS += \
./src/MCU.o \
./src/RTC.o \
./src/SLEEP.o \
./src/app_bleCmdHandle.o \
./src/drawing.o \
./src/eeprom.o \
./src/epd.o \
./src/epd_ssd.o \
./src/epd_uc.o \
./src/gattprofile.o \
./src/main.o \
./src/nfc.o \
./src/peripheral.o \
./src/settings.o 



# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -g -DDEBUG=1 -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver/inc" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/RVMSIS" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/src" -I"c:/Users/pc/Documents/GitHub/ATC_BLE_OEPL_CH573/ATC_BLE_OEPL_CH573F_MounRiverProject/StdPeriphDriver" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
