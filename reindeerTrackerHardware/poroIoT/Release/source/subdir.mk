################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/main.c 

OBJS += \
./source/main.o 

C_DEPS += \
./source/main.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g -DCPU_MKL17Z256VFM4 -DCPU_MKL17Z256VFM4_cm0plus -DFSL_RTOS_BM -DNDEBUG -DSDK_OS_BAREMETAL -I../CMSIS -I../board -I../drivers -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


