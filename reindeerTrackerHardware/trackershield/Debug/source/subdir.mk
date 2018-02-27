################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/acc_func.c \
../source/adc_func.c \
../source/i2c_func.c \
../source/main.c 

OBJS += \
./source/acc_func.o \
./source/adc_func.o \
./source/i2c_func.o \
./source/main.o 

C_DEPS += \
./source/acc_func.d \
./source/adc_func.d \
./source/i2c_func.d \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -DCPU_MK64FN1M0VDC12_cm4 -DFSL_RTOS_BM -DCPU_MK64FN1M0VDC12 -DSDK_OS_BAREMETAL -I../CMSIS -I../board -I../drivers -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


