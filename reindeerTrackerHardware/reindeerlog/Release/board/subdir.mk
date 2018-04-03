################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/peripherals.c \
../board/pin_mux.c 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/peripherals.o \
./board/pin_mux.o 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/peripherals.d \
./board/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DCPU_MK64FN1M0VDC12 -Du_printf_float -DCPU_MK64FN1M0VDC12_cm4 -DFSL_RTOS_BM -DNDEBUG -DSDK_OS_BAREMETAL -I../CMSIS -I/media/teemu/ubuntu2/Git/IP18-ReindeerTracker02/reindeerTrackerHardware/K64f_lib/SDK_2.3.0_FRDM-K64F/middleware/fatfs/src/fsl_sd_disk -I/media/teemu/ubuntu2/Git/IP18-ReindeerTracker02/reindeerTrackerHardware/K64f_lib/SDK_2.3.0_FRDM-K64F/middleware/sdmmc/src -I/media/teemu/ubuntu2/Git/IP18-ReindeerTracker02/reindeerTrackerHardware/K64f_lib/SDK_2.3.0_FRDM-K64F/middleware/sdmmc/inc -I/home/nks/IP18-ReindeerTracker02/reindeerTrackerHardware/K64lib/SDK_2.3.0_FRDM-K64F/middleware/sdmmc/inc -I/home/nks/IP18-ReindeerTracker02/reindeerTrackerHardware/K64lib/SDK_2.3.0_FRDM-K64F/middleware/sdmmc/src -I/home/nks/IP18-ReindeerTracker02/reindeerTrackerHardware/K64lib/SDK_2.3.0_FRDM-K64F/middleware/fatfs/src/fsl_sd_disk -I/home/pauli/reindeertracker/IP18-ReindeerTracker02/reindeerTrackerHardware/kinetis_K64_lib/middleware/sdmmc/src -I/home/pauli/reindeertracker/IP18-ReindeerTracker02/reindeerTrackerHardware/kinetis_K64_lib/middleware/sdmmc/inc -I../board -I../drivers -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


