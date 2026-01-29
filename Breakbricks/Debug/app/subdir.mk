################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/breakbricks.c \
../app/button.c \
../app/display.c \
../app/main.c 

OBJS += \
./app/breakbricks.o \
./app/button.o \
./app/display.o \
./app/main.o 

C_DEPS += \
./app/breakbricks.d \
./app/button.d \
./app/display.d \
./app/main.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o app/%.su app/%.cyclo: ../app/%.c app/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G431xx -c -I../core/Inc -I../../common/core/Inc -I../drivers/stm32g4xx_hal/Inc -I../../common/drivers/stm32g4xx_hal/Inc -I../drivers/stm32g4xx_hal/Inc/Legacy -I../../common/drivers/stm32g4xx_hal/Inc/Legacy -I../drivers/cmsis/Device/ST/STM32G4xx/Include -I../../common/drivers/cmsis/Device/ST/STM32G4xx/Include -I../drivers/cmsis/Include -I../../common/drivers/cmsis/Include -I../app -I../drivers/bsp -I../../common/drivers/bsp -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-app

clean-app:
	-$(RM) ./app/breakbricks.cyclo ./app/breakbricks.d ./app/breakbricks.o ./app/breakbricks.su ./app/button.cyclo ./app/button.d ./app/button.o ./app/button.su ./app/display.cyclo ./app/display.d ./app/display.o ./app/display.su ./app/main.cyclo ./app/main.d ./app/main.o ./app/main.su

.PHONY: clean-app

